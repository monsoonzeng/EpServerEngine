/*! 
@file epBaseClient.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base Client Interface
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

An Interface for Base Client.

*/
#ifndef __EP_BASE_CLIENT_H__
#define __EP_BASE_CLIENT_H__

#include "epServerEngine.h"
#include "epPacket.h"
#include "epBaseServerSendObject.h"
#include "epBasePacketParser.h"
#include "epServerConf.h"
#include "epParserList.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif //WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>



// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")



using namespace std;


namespace epse{

	/*! 
	@class BaseClient epBaseClient.h
	@brief A class for Base Client.
	*/
	class EP_SERVER_ENGINE BaseClient:public BaseServerSendObject{
	public:
		/*!
		Default Constructor

		Initializes the Client
		@param[in] hostName the hostname string
		@param[in] port the port string
		@param[in] syncPolicy Synchronous Policy
		@param[in] maximumParserCount the maximum number of parser
		@param[in] waitTimeMilliSec wait time for Client Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseClient(const TCHAR * hostName=_T(DEFAULT_HOSTNAME), const TCHAR * port=_T(DEFAULT_PORT),SyncPolicy syncPolicy=SYNC_POLICY_ASYNCHRONOUS,unsigned int maximumParserCount=PARSER_LIMIT_INFINITE,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseClient
		@param[in] b the second object
		*/
		BaseClient(const BaseClient& b);
		/*!
		Default Destructor

		Destroy the Client
		*/
		virtual ~BaseClient();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseClient & operator=(const BaseClient&b);
	

		/*!
		Set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void SetHostName(const TCHAR * hostName);

		/*!
		Set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void SetPort(const TCHAR * port);

		/*!
		Get the hostname of server
		@return the hostname in string
		*/
		epl::EpTString GetHostName() const;

		/*!
		Get the port number of server
		@return the port number in string
		*/
		epl::EpTString GetPort() const;

		/*!
		Set the Maximum Parser Count for the server.
		@param[in] maxParserCount The Maximum Parser Count to set.
		@remark 0 means there is no limit
		*/
		void SetMaximumParserCount(unsigned int maxParserCount);

		/*!
		Get the Maximum Parser Parser of server
		@return The Maximum Connection Count
		@remark 0 means there is no limit
		*/
		unsigned int GetMaximumParserCount() const;

		/*!
		Set Synchronous Policy
		@param[in] syncPolicy Synchronous Policy to set
		@return true if successfully set otherwise false
		@remark SyncPolicy cannot be set when Client is connected to the server.
		*/
		bool SetSyncPolicy(SyncPolicy syncPolicy);

		/*!
		Get current Synchronous Policy
		@return Synchronous Policy
		*/
		SyncPolicy GetSyncPolicy() const;

		/*!
		Set the wait time for the thread termination
		@param[in] milliSec the time for waiting in millisecond
		*/
		virtual void SetWaitTime(unsigned int milliSec);

		/*!
		Connect to the server
		@param[in] hostName the hostname string
		@param[in] port the port string
		@remark if argument is NULL then previously setting value is used
		*/
		bool Connect(const TCHAR * hostName=NULL, const TCHAR * port=NULL);

		/*!
		Disconnect from the server
		*/
		void Disconnect();

		/*!
		Check if the connection is established
		@return true if the connection is established otherwise false
		*/
		bool IsConnected() const;

		/*!
		Send the packet to the server
		@param[in] packet the packet to be sent
		@param[in] waitTimeInMilliSec wait time for sending the packet in millisecond
		@return sent byte size
		@remark return -1 if error occurred
		*/
		virtual int Send(const Packet &packet, unsigned int waitTimeInMilliSec=WAITTIME_INIFINITE);

		/*!
		Get Packet Parser List
		@return the list of the packet parser
		*/
		vector<BaseServerObject*> GetPacketParserList() const;

	protected:
		/*!
		Return the new packet parser
		@remark Sub-class should implement this to create new parser.
		@remark Client will automatically release this parser.
		@return the new packet parser
		*/
		virtual BasePacketParser* createNewPacketParser()=0;

	private:

		/*!
		Reset Client
		*/
		void resetClient();
		/*!
		Actually set the hostname for the server.
		@remark Cannot be changed while connected to server
		@param[in] hostName The hostname to set.
		*/
		void setHostName(const TCHAR * hostName);
		
		/*!
		Actually set the port for the server.
		@remark Cannot be changed while connected to server
		@param[in] port The port to set.
		*/
		void setPort(const TCHAR *port);

		/*!
		Receive the packet from the server
		@param[out] packet the packet received
		@return received byte size
		*/
		int receive(Packet &packet);

		/*!
		Actually processing the client thread
		*/
		virtual void execute();


	
		/*!
		Clean up the client initialization.
		*/
		void cleanUpClient();

		/*!
		Actually Disconnect from the server
		*/
		void disconnect();


	private:
		/// port
		epl::EpString m_port;
		/// hostname
		epl::EpString m_hostName;
		/// connection socket
		SOCKET m_connectSocket;
		/// internal variable
		struct addrinfo *m_result;
	
		/// send lock
		epl::BaseLock *m_sendLock;
		/// general lock
		epl::BaseLock *m_generalLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// Temp Packet;
		Packet m_recvSizePacket;

		/// Parser list
		ParserList m_parserList;

		/// Maximum Parser Count
		unsigned int m_maxParserCount;

	};
}


#endif //__EP_BASE_CLIENT_H__