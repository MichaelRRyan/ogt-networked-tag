#include "Server.h"
#include <WS2tcpip.h>
#include <iostream>
#include "PacketStructs.h"
#pragma comment(lib,"ws2_32.lib") //Required for WinSock


Server::Server(int port, bool loopBacktoLocalHost) //Port = port to broadcast on. loopBacktoLocalHost = true if loopback to local host (nobody else can connect), false if loop back to any addr (anyone can connect) [assumes port is properly forwarded in router settings)
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0) //If WSAStartup returns anything other than 0, then that means an error has occured in the WinSock Startup.
	{
		MessageBoxA(0, "WinSock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	if (loopBacktoLocalHost) //If server is to loopback to local host (others cannot connect)
		inet_pton(AF_INET, "127.0.0.1", &m_addr.sin_addr.s_addr);
	else
		m_addr.sin_addr.s_addr = htonl(INADDR_ANY); //loopback to any addr so others can connect

	m_addr.sin_port = htons(port); //Port
	m_addr.sin_family = AF_INET; //IPv4 Socket

	m_sListen = socket(AF_INET, SOCK_STREAM, 0); //Create socket to listen for new connections
	if (bind(m_sListen, (SOCKADDR*)&m_addr, sizeof(m_addr)) == SOCKET_ERROR) //Bind the address to the socket, if we fail to bind the address..
	{
		std::string ErrorMsg = "Failed to bind the address to our listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(0, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	if (listen(m_sListen, SOMAXCONN) == SOCKET_ERROR) //Places sListen socket in a state in which it is listening for an incoming connection. Note:SOMAXCONN = Socket Oustanding Max connections, if we fail to listen on listening socket...
	{
		std::string ErrorMsg = "Failed to listen on listening socket. Winsock Error:" + std::to_string(WSAGetLastError());
		MessageBoxA(0, ErrorMsg.c_str(), "Error", MB_OK | MB_ICONERROR);
		exit(1);
	}
	m_IDCounter = 1;
	std::thread PST(PacketSenderThread, std::ref(*this));
	PST.detach();
	m_threads.push_back(&PST);

	std::thread LFNCT(NewConnectionThread, std::ref(*this));
	LFNCT.detach();
	m_threads.push_back(&LFNCT);
}

Server::~Server()
{
	m_terminateThreads = true;
	for (std::thread* t : m_threads) //Wait for all created threads to end...
	{
		t->join();
	}
}

void Server::setPlayerPosition(char id, int x, int y)
{
	std::string message{ "   " };
	message.at(0) = id;
	message.at(1) = (char)x;
	message.at(2) = (char)y;

	std::shared_ptr<Packet> p = std::make_shared<Packet>();
	p->Append(PacketType::MovePlayer);
	p->Append(message.size());
	p->Append(message);

	for (auto conn : m_connections)
		conn->m_pm.Append(p);
}

void Server::NewConnectionThread(Server & server)
{
	while (!server.m_terminateThreads)
	{
		int addrlen = sizeof(m_addr);
		SOCKET newConnectionSocket = accept(server.m_sListen, (SOCKADDR*)&server.m_addr, &addrlen); //Accept a new connection
		if (newConnectionSocket == 0) //If accepting the client connection failed
		{
			std::cout << "Failed to accept the client's connection." << std::endl;
		}
		else //If client connection properly accepted
		{
			std::lock_guard<std::shared_mutex> lock(server.m_mutex_connectionMgr); //Lock connection manager mutex since we are adding an element to connection vector
			std::shared_ptr<Connection> newConnection(std::make_shared<Connection>(newConnectionSocket));
			server.m_connections.push_back(newConnection); //push new connection into vector of connections
			newConnection->m_ID = server.m_IDCounter; //Set ID for this connection
			server.m_IDCounter += 1; //Increment ID Counter...
			std::cout << "Client Connected! ID:" << newConnection->m_ID << std::endl;
			std::thread CHT(ClientHandlerThread, std::ref(server), newConnection);
			CHT.detach();
			server.m_threads.push_back(&CHT);

			// Sends the new client their ID.
			std::string info{ "" };
			info += static_cast<char>(newConnection->m_ID); // ID
			info += static_cast<char>(7 + newConnection->m_ID); // Tile x
			info += static_cast<char>(2); // Tile y
			std::shared_ptr<Packet> p = std::make_shared<Packet>();
			p->Append(PacketType::JoinInfo);
			p->Append(info.size());
			p->Append(info);
			newConnection->m_pm.Append(p);

			std::shared_ptr<Packet> p2 = std::make_shared<Packet>();
			p2->Append(PacketType::PlayerJoined);
			p2->Append(info.size());
			p2->Append(info);
			
			// Updates all other players of the new joiner.
			for (auto conn : server.m_connections) //For each connection...
			{
				if (conn == newConnection) //If connection is the user who sent the message...
					continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.
				conn->m_pm.Append(p2);
			}
		}
	}
}

bool Server::ProcessPacket(std::shared_ptr<Connection> connection, PacketType packetType)
{
	switch (packetType)
	{
	case PacketType::RequestToMove:
	{
		std::string message; 
		if (!getString(connection->m_socket, message))
			return false; 

		// Adds the ID to the start.
		message = " " + message;
		message.at(0) = connection->m_ID;

		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::MovePlayer);
		p->Append(message.size());
		p->Append(message);

		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
			for (auto conn : m_connections) //For each connection...
			{
				//if (conn == connection) //If connection is the user who sent the message...
				//	continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.
				conn->m_pm.Append(p);
			}
		}
		std::cout << "Processed player position packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::ChatMessage: //Packet Type: chat message
	{
		std::string message; //string to store our message we received
		if (!getString(connection->m_socket, message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
						  //Next we need to send the message out to each user

		PS::ChatMessage cm(message);
		std::shared_ptr<Packet> msgPacket = std::make_shared<Packet>(cm.toPacket()); //use shared_ptr instead of sending with SendString so we don't have to reallocate packet for each connection
		{
			std::shared_lock<std::shared_mutex> lock(m_mutex_connectionMgr);
			for (auto conn : m_connections) //For each connection...
			{
				if (conn == connection) //If connection is the user who sent the message...
					continue;//Skip to the next user since there is no purpose in sending the message back to the user who sent it.
				conn->m_pm.Append(msgPacket);
			}
		}
		std::cout << "Processed chat message packet from user ID: " << connection->m_ID << std::endl;
		break;
	}
	case PacketType::FileTransferRequestFile:
	{
		std::string fileName; //string to store file name
		if (!getString(connection->m_socket, fileName)) //If issue getting file name
			return false; //Failure to process packet
		std::string errMsg;
		if (connection->m_file.Initialize(fileName, errMsg)) //if filetransferdata successfully initialized
		{
			connection->m_pm.Append(connection->m_file.getOutgoingBufferPacket()); //Send first buffer from file
		}
		else //If initialization failed, send error message
		{
			sendString(connection->m_pm, PacketType::ChatMessage, errMsg);
		}
		break;
	}
	case PacketType::FileTransferRequestNextBuffer:
	{
		if (connection->m_file.m_transferInProgress)
		{
			connection->m_pm.Append(connection->m_file.getOutgoingBufferPacket()); //get and send next buffer for file
		}
		break;
	}
	default: //If packet type is not accounted for
	{
		std::cout << "Unrecognized packet: " << (std::int32_t)packetType << std::endl; //Display that packet was not found
		return false;
	}
	}
	return true;
}

void Server::ClientHandlerThread(Server & server, std::shared_ptr<Connection> connection) //ID = the index in the SOCKET connections array
{
	PacketType packettype;
	while (true)
	{
		if (server.m_terminateThreads == true)
			break;
		if (!server.getPacketType(connection->m_socket, packettype)) //Get packet type
			break; //If there is an issue getting the packet type, exit this loop
		if (!server.ProcessPacket(connection, packettype)) //Process packet (packet type)
			break; //If there is an issue processing the packet, exit this loop
	}
	std::cout << "Lost connection to client ID: " << connection->m_ID << std::endl;
	server.DisconnectClient(connection); //Disconnect this client and clean up the connection if possible
	return;
}

void Server::PacketSenderThread(Server & server) //Thread for all outgoing packets
{
	while (true)
	{
		if (server.m_terminateThreads == true)
			break;

		std::shared_lock<std::shared_mutex> lock(server.m_mutex_connectionMgr);
		for (auto conn : server.m_connections) //for each connection...
		{
			if (conn->m_pm.HasPendingPackets()) //If there are pending packets for this connection's packet manager
			{
				std::shared_ptr<Packet> p = conn->m_pm.Retrieve(); //Retrieve packet from packet manager
				if (!server.sendAll(conn->m_socket, (const char*)(&p->m_buffer[0]), p->m_buffer.size())) //send packet to connection
				{
					std::cout << "Failed to send packet to ID: " << conn->m_ID << std::endl; //Print out if failed to send packet
				}
			}
		}
		Sleep(5);
	}
	std::cout << "Ending Packet Sender Thread..." << std::endl;
}

void Server::DisconnectClient(std::shared_ptr<Connection> connection) //Disconnects a client and cleans up socket if possible
{
	std::lock_guard<std::shared_mutex> lock(m_mutex_connectionMgr); //Lock connection manager mutex since we are possible removing element(s) from the vector
	connection->m_pm.Clear(); //Clear out all remaining packets in queue for this connection
	closesocket(connection->m_socket); //Close the socket for this connection
	m_connections.erase(std::remove(m_connections.begin(), m_connections.end(), connection)); //Remove connection from vector of connections
	std::cout << "Cleaned up client: " << connection->m_ID << "." << std::endl;
	std::cout << "Total connections: " << m_connections.size() << std::endl;
}