/*! 
@file epIocpTcpSocket.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief IOCP TCP Socket Interface
@version 1.0

@section LICENSE

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

@section DESCRIPTION

An Interface for IOCP TCP Socket.

*/
#ifndef __EP_IOCP_TCP_SOCKET_H__
#define __EP_IOCP_TCP_SOCKET_H__

#include "epServerEngine.h"
#include "epBaseTcpSocket.h"

namespace epse
{

	/*! 
	@class IocpTcpSocket epIocpTcpSocket.h
	@brief A class for IOCP TCP Socket.
	*/
	class EP_SERVER_ENGINE IocpTcpSocket:public BaseTcpSocket
	{
	public:
		/*!
		Default Constructor

		Initializes the Socket
		@param[in] callBackObj the callback object
		@param[in] waitTimeMilliSec wait time for Socket Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		IocpTcpSocket(ServerCallbackInterface *callBackObj,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Destructor

		Destroy the Socket
		*/
		virtual ~IocpTcpSocket();

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		bool IsConnectionAlive() const;

		/*!
		Kill the connection
		*/
		void KillConnection(EventEx *completionEvent,ServerCallbackInterface *callBackObj,Priority priority);

		/*!
		Kill the connection
		*/
		void KillConnection();

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] complentionEvent the event to set when send is completed
		@param[in] callBackObj the object to callback when send is completed
		@param[in] priority the priority of the send job
		@remark if the completionEvent and callBackObj are set as NULL, it is ignored.
		*/
		void Send(Packet &packet,EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL);


		/*!
		Receive the packet from the client
		@param[in] complentionEvent the event to set when receive is completed
		@param[in] callBackObj the object to callback when receive is completed
		@param[in] priority the priority of the receive job
		@remark if the completionEvent and callBackObj are set as NULL, it is ignored.
		*/
		void Receive(EventEx *completionEvent=NULL,ServerCallbackInterface *callBackObj=NULL,Priority priority=PRIORITY_NORMAL);

	private:	
		friend class IocpTcpServer;
		friend class IocpTcpProcessor;

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,SendStatus *sendStatus=NULL);


		/*!
		Receive the packet from the client
		@param[in] waitTimeInMilliSec wait time for receiving the packet in millisecond
		@param[out] retStatus the pointer to ReceiveStatus enumerator to get receive status.
		@return received packet
		@remark the caller must call ReleaseObj() for Packet to avoid the memory leak.
		*/
		Packet *Receive(unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE,ReceiveStatus *retStatus=NULL);
	

		/*!
		Actually Kill the connection
		*/
		virtual void killConnection();

		/*!
		Actually Kill the connection without Callback
		*/
		void killConnectionNoCallBack();

		/*!
		thread loop function
		*/
		virtual void execute();

		
	private:
		/*!
		Default Copy Constructor

		Initializes the Socket
		@param[in] b the second object
		@remark Copy Constructor prohibited
		*/
		IocpTcpSocket(const IocpTcpSocket& b):BaseTcpSocket(b)
		{}

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		@remark Copy Operator prohibited
		*/
		IocpTcpSocket & operator=(const IocpTcpSocket&b){return *this;}
	
	
	private:

		/// Connection status
		bool m_isConnected;
	};

}

#endif //__EP_IOCP_TCP_SOCKET_H__