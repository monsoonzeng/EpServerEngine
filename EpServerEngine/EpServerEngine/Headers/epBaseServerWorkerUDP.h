/*! 
@file epBaseServerWorkerUDP.h
@author Woong Gyu La a.k.a Chris. <juhgiyo@gmail.com>
		<http://github.com/juhgiyo/epserverengine>
@date February 13, 2012
@brief Base UDP Worker Interface
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

An Interface for Base UDP Server Worker.

*/
#ifndef __EP_BASE_SERVER_WORKER_UDP_H__
#define __EP_BASE_SERVER_WORKER_UDP_H__

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



// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

namespace epse
{
	class BaseServerUDP;
	/*! 
	@class BaseServerWorkerUDP epBaseServerWorkerUDP.h
	@brief A class for Base UDP Server Worker.
	*/
	class EP_SERVER_ENGINE BaseServerWorkerUDP:public BaseServerSendObject
	{
		friend class BaseServerUDP;
	public:
		/*!
		Default Constructor

		Initializes the Worker
		@param[in] maximumParserCount the maximum number of parser
		@param[in] waitTimeMilliSec wait time for Worker Thread to terminate
		@param[in] lockPolicyType The lock policy
		*/
		BaseServerWorkerUDP(unsigned int maximumParserCount=PARSER_LIMIT_INFINITE,unsigned int waitTimeMilliSec=WAITTIME_INIFINITE,epl::LockPolicy lockPolicyType=epl::EP_LOCK_POLICY);

		/*!
		Default Copy Constructor

		Initializes the BaseServerWorkerUDP
		@param[in] b the second object
		*/
		BaseServerWorkerUDP(const BaseServerWorkerUDP& b);

		/*!
		Default Destructor

		Destroy the Worker
		*/
		virtual ~BaseServerWorkerUDP();

		/*!
		Assignment operator overloading
		@param[in] b the second object
		@return the new copied object
		*/
		BaseServerWorkerUDP & operator=(const BaseServerWorkerUDP&b);

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
		Send the packet to the client
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

		/*!
		Check if the connection is alive
		@return true if the connection is alive otherwise false
		*/
		bool IsConnectionAlive() const;

		/*!
		Kill the connection
		*/
		void KillConnection();


		/*!
		Get the owner object of this worker object.
		@return the pointer to the owner object.
		*/
		BaseServerUDP *GetOwner() const;

		/*!
		Get the maximum packet byte size
		@return the maximum packet byte size
		*/
		unsigned int GetMaxPacketByteSize() const;

	protected:
		/*!
		Return the new packet parser
		@remark Sub-class should implement this to create new parser.
		@remark Client will automatically release this parser.
		@return the new packet parser
		*/
		virtual BasePacketParser* createNewPacketParser()=0;

	private:	
		friend class BaseServerUDP;
		/*!
		Set Parser List for current worker
		@param[in] parserList the parser list to set
		*/
		void setParserList(ParserList *parserList);

		/*!
		Add new packet received from client
		@param[in] packet the new packet received from client
		*/
		void addPacket(Packet *packet);
	
		/*!
		Reset worker
		*/
		void resetWorker();
		/*!
		Actually Kill the connection
		*/
		void killConnection();

		/*! 
		@struct PacketPassUnit epBaseServerWorkerUDP.h
		@brief A class for Packet Passing Unit for Packet Parsing Thread.
		*/
		struct PacketPassUnit{
			/// BaseServerUDP Object
			BaseServerUDP *m_server;
			/// client socket
			sockaddr m_clientSocket;
		};

		/*!
		thread loop function
		*/
		virtual void execute();

		/*!
		Set the server object as given
		@param[in] server the pointer to the server object
		*/
		void setServer(BaseServerUDP *server);

		/*!
		Set PacketPassUnit
		@param[in] packetPassUnit PacketPassUnit to set
		*/
		void setPacketPassUnit(const PacketPassUnit& packetPassUnit);	

	
	private:
	
		/// client socket
		sockaddr m_clientSocket;

		/// server object
		BaseServerUDP *m_server;

		/// Packet List
		vector<Packet*> m_packetList;

		/// Maximum UDP Datagram byte size
		unsigned int m_maxPacketSize;

		/// general lock 
		epl::BaseLock *m_baseWorkerLock;

		/// list lock 
		epl::BaseLock *m_listLock;

		/// Lock Policy
		epl::LockPolicy m_lockPolicy;

		/// parser thread list
		ParserList *m_parserList;

		/// Maximum Parser Count
		unsigned int m_maxParserCount;

		/// Thread Stop Event
		/// @remark if this is raised, the thread should quickly stop.
		epl::EventEx m_threadStopEvent;
	};

}

#endif //__EP_BASE_SERVER_WORKER_UDP_H__