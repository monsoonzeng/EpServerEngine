/*! 
SyncUdpSocket for the EpServerEngine
Copyright (C) 2012  Woong Gyu La <juhgiyo@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "epSyncUdpSocket.h"
#include "epSyncUdpServer.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;
SyncUdpSocket::SyncUdpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec,epl::LockPolicy lockPolicyType): BaseUdpSocket(callBackObj,waitTimeMilliSec,lockPolicyType)
{
	m_packetReceivedEvent=EventEx(false,false);
	m_isConnected=true;
}

SyncUdpSocket::~SyncUdpSocket()
{
	KillConnection();
}

bool SyncUdpSocket::IsConnectionAlive() const
{
	return m_isConnected;
}

void SyncUdpSocket::KillConnection()
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		return;
	}
	m_isConnected=false;

	m_listLock->Lock();
	Packet *removeElem=NULL;
	while(!m_packetList.empty())
	{
		removeElem=m_packetList.front();
		if(removeElem)
			removeElem->ReleaseObj();
		m_packetList.pop();
	}
	m_listLock->Unlock();

	removeSelfFromContainer();
	m_callBackObj->OnDisconnect(this);
}


void SyncUdpSocket::killConnection()
{
	if(IsConnectionAlive())
	{
		m_isConnected=false;

		m_listLock->Lock();
		Packet *removeElem=NULL;
		while(!m_packetList.empty())
		{
			removeElem=m_packetList.front();
			if(removeElem)
				removeElem->ReleaseObj();
			m_packetList.pop();
		}
		m_listLock->Unlock();


		removeSelfFromContainer();
		m_callBackObj->OnDisconnect(this);

	}
}

void SyncUdpSocket::addPacket(Packet *packet)
{
	if(packet)
		packet->RetainObj();
	epl::LockObj lock(m_listLock);
	m_packetList.push(packet);
	m_packetReceivedEvent.SetEvent();
}

int SyncUdpSocket::Send(const Packet &packet, unsigned int waitTimeInMilliSec,SendStatus *sendStatus)
{
	epl::LockObj lock(m_baseSocketLock);
	return BaseUdpSocket::Send(packet,waitTimeInMilliSec,sendStatus);
}

Packet *SyncUdpSocket::Receive(unsigned int waitTimeInMilliSec,ReceiveStatus *retStatus)
{
	epl::LockObj lock(m_baseSocketLock);
	if(!IsConnectionAlive())
	{
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_NOT_CONNECTED;
		return NULL;
	}

	// receive routine
	unsigned int packetSize=0;

	m_listLock->Lock();
	if(m_packetList.size()==0)
	{
		m_listLock->Unlock();
		if(!m_packetReceivedEvent.WaitForEvent(waitTimeInMilliSec))
		{
			if(retStatus)
				*retStatus=RECEIVE_STATUS_FAIL_TIME_OUT;
			return NULL;
		}

	}
	Packet *packet= m_packetList.front();
	packetSize=packet->GetPacketByteSize();
	if(packetSize==0)
	{
		m_listLock->Unlock();
		killConnection();
		if(retStatus)
			*retStatus=RECEIVE_STATUS_FAIL_CONNECTION_CLOSING;
		return NULL;
	}
	m_packetList.pop();
	m_listLock->Unlock();
	if(retStatus)
		*retStatus=RECEIVE_STATUS_SUCCESS;
	return packet;
}

void SyncUdpSocket::execute()
{
	m_callBackObj->OnNewConnection(this);
}

