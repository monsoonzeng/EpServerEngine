/*! 
AsyncUdpServer for the EpServerEngine
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
#include "epAsyncUdpServer.h"
#include "epAsyncUdpSocket.h"

#if defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif // defined(_DEBUG) && defined(EP_ENABLE_CRTDBG)

using namespace epse;

AsyncUdpServer::AsyncUdpServer(epl::LockPolicy lockPolicyType): BaseUdpServer(lockPolicyType)
{
	m_isAsynchronousReceive=true;
}


AsyncUdpServer::AsyncUdpServer(const AsyncUdpServer& b):BaseUdpServer(b)
{
	LockObj lock(b.m_baseServerLock);
	m_isAsynchronousReceive=b.m_isAsynchronousReceive;
}
AsyncUdpServer::~AsyncUdpServer()
{
}
AsyncUdpServer & AsyncUdpServer::operator=(const AsyncUdpServer&b)
{
	if(this!=&b)
	{
	
		BaseUdpServer::operator =(b);
		LockObj lock(b.m_baseServerLock);
		m_isAsynchronousReceive=b.m_isAsynchronousReceive;
	}
	return *this;
}

bool AsyncUdpServer::GetIsAsynchronousReceive() const
{
	return m_isAsynchronousReceive;
}
void AsyncUdpServer::SetIsAsynchronousReceive(bool isASynchronousReceive)
{
	m_isAsynchronousReceive=isASynchronousReceive;
}

bool AsyncUdpServer::StartServer(const ServerOps &ops)
{
	m_isAsynchronousReceive=ops.isAsynchronousReceive;
	return BaseUdpServer::StartServer(ops);
}

void AsyncUdpServer::execute()
{
	Packet recvPacket(NULL,m_maxPacketSize);
	char *packetData=const_cast<char*>(recvPacket.GetPacket());
	int length=recvPacket.GetPacketByteSize();
	sockaddr clientSockAddr;
	int sockAddrSize=sizeof(sockaddr);
	while(m_listenSocket!=INVALID_SOCKET)
	{
		int recvLength=recvfrom(m_listenSocket,packetData,length, 0,&clientSockAddr,&sockAddrSize);

		AsyncUdpSocket *workerObj=(AsyncUdpSocket*)m_socketList.Find(clientSockAddr,socketCompare);
		if(workerObj)
		{
			if(recvLength<=0)
			{
				Packet *passPacket=EP_NEW Packet(packetData,0);
				workerObj->addPacket(passPacket);
				passPacket->ReleaseObj();
				continue;
			}	
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			workerObj->addPacket(passPacket);
			passPacket->ReleaseObj();
		}
		else
		{
			if(recvLength<=0)
				continue;
			if(GetMaximumConnectionCount()!=CONNECTION_LIMIT_INFINITE)
			{
				if(m_socketList.Count()>=GetMaximumConnectionCount())
				{
					continue;
				}
			}
			if(!m_callBackObj->OnAccept(clientSockAddr))
			{
				continue;
			}
			/// Create Worker Thread
			Packet *passPacket=EP_NEW Packet(packetData,recvLength);
			AsyncUdpSocket *accWorker=EP_NEW AsyncUdpSocket(m_callBackObj,m_isAsynchronousReceive,m_waitTime,PROCESSOR_LIMIT_INFINITE,m_lockPolicy);
			accWorker->setSockAddr(clientSockAddr);
			accWorker->setOwner(this);
			accWorker->setMaxPacketByteSize(m_maxPacketSize);
			m_socketList.Push(accWorker);
			accWorker->Start();
			accWorker->addPacket(passPacket);
			accWorker->ReleaseObj();
			passPacket->ReleaseObj();

		}

	}

	stopServer();
} 

