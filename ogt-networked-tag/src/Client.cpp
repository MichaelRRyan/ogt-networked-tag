#include "Client.h"
#include <Ws2tcpip.h> //for inet_pton
#pragma comment(lib,"ws2_32.lib") //Required for WinSock
#include <iostream> //for std::cout
#include "PacketStructs.h"

Client::Client(const char* ip, const int port)
{
	//Winsock Startup
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		MessageBoxA(0, "Winsock startup failed", "Error", MB_OK | MB_ICONERROR);
		exit(0);
	}


	inet_pton(AF_INET, ip, &m_addr.sin_addr.s_addr); //Server Address [127.0.0.1] = localhost
	m_addr.sin_port = htons(port); //Port 
	m_addr.sin_family = AF_INET; //IPv4 Socket
}

bool Client::Connect()
{
	m_connection = socket(AF_INET, SOCK_STREAM, 0); //Set Connection socket
	int sizeofaddr = sizeof(m_addr);
	if (connect(m_connection, (SOCKADDR*)&m_addr, sizeofaddr) != 0) //If we are unable to connect...
	{
		MessageBoxA(0, "Failed to Connect", "Error", MB_OK | MB_ICONERROR);
		return false;
	}

	std::cout << "Connected!" << std::endl;
	m_pst = std::thread(PacketSenderThread, std::ref(*this)); //Create thread to send packets
	m_pst.detach();
	m_ct = std::thread(ClientThread, std::ref(*this)); //Create thread to listen to server
	m_ct.detach();
	return true;
}

bool Client::CloseConnection()
{
	m_terminateThreads = true;
	if (closesocket(m_connection) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAENOTSOCK) //If socket error is that operation is not performed on a socket (This happens when the socket has already been closed)
			return true; //return true since connection has already been closed

		std::string ErrorMessage = "Failed to close the socket. Winsock Error: " + std::to_string(WSAGetLastError()) + ".";
		MessageBoxA(0, ErrorMessage.c_str(), "Error", MB_OK | MB_ICONERROR);
		return false;
	}
	return true;
}

Client::~Client()
{
	CloseConnection();
	m_pst.join();
	m_ct.join();
}

bool Client::ProcessPacketType(PacketType packetType)
{
	/*
	MovePlayer,
	PlayerDied,
	RockMoved,
	*/
	switch (packetType)
	{
	case PacketType::GameStarted:
	case PacketType::GameEnded:
	case PacketType::JoinInfo:
	case PacketType::MovePlayer:
	case PacketType::PlayerDied:
	case PacketType::RockMoved:
	{
		std::string message;
		if (!getString(m_connection, message))
			return false;

		m_packetRecievedCallback(packetType, message);
		break;
	}
	//case PacketType::PlayerPosition: //If PacketType is a PlayerPosition
	//{
	//	std::string Message; //string to store our message we received
	//	if (!getString(m_connection, Message)) //Get the chat message and store it in variable: Message
	//		return false; //If we do not properly get the chat message, return false
	//	
	//	if (Message.size() == 3)
	//		m_playerPositionCallback(Message.at(0), Message.at(1), Message.at(2));

	//	break;
	//}
	case PacketType::ChatMessage: //If PacketType is a chat message PacketType
	{
		std::string Message; //string to store our message we received
		if (!getString(m_connection, Message)) //Get the chat message and store it in variable: Message
			return false; //If we do not properly get the chat message, return false
		std::cout << Message << std::endl; //Display the message to the user
		break;
	}
	case PacketType::FileTransferByteBuffer:
	{
		std::int32_t buffersize; //buffer to hold size of buffer to write to file
		if (!getInt32_t(m_connection, buffersize)) //get size of buffer as integer
			return false;
		if (buffersize > FileTransferData::m_bufferSize) //If invalid buffer size (too large)
			return false;
		if (!recieveAll(m_connection, m_file.m_buffer, buffersize)) //get buffer and store it in file.buffer
			return false;
		m_file.m_outfileStream.write(m_file.m_buffer, buffersize); //write buffer from file.buffer to our outfilestream
		m_file.m_bytesWritten += buffersize; //increment byteswritten
		std::cout << "Received byte buffer for file transfer of size: " << buffersize << std::endl;
		m_pm.Append(std::make_shared<Packet>(Packet(PacketType::FileTransferRequestNextBuffer)));
		break;
	}
	case PacketType::FileTransfer_EndOfFile:
	{
		std::cout << "File transfer completed. File received.\n";
		std::cout << "File Name: " << m_file.m_fileName << "\n";
		std::cout << "File Size(bytes): " << m_file.m_bytesWritten << std::endl;
		m_file.m_transferInProgress = false;
		m_file.m_bytesWritten = 0;
		m_file.m_outfileStream.close(); //close file after we are done writing file
		break;
	}
	default: //If PacketType type is not accounted for
		std::cout << "Unrecognized PacketType: " << (std::int32_t)packetType << std::endl; //Display that PacketType was not found
		break;
	}
	return true;
}

void Client::ClientThread(Client & client)
{
	PacketType PacketType;
	while (true)
	{
		if (client.m_terminateThreads == true)
			break;
		if (!client.getPacketType(client.m_connection, PacketType)) //Get PacketType type
			break; //If there is an issue getting the PacketType type, exit this loop
		if (!client.ProcessPacketType(PacketType)) //Process PacketType (PacketType type)
			break; //If there is an issue processing the PacketType, exit this loop
	}
	std::cout << "Lost connection to the server.\n";
	client.m_terminateThreads = true;
	if (client.CloseConnection()) //Try to close socket connection..., If connection socket was closed properly
	{
		std::cout << "Socket to the server was closed successfully." << std::endl;
	}
	else //If connection socket was not closed properly for some reason from our function
	{
		std::cout << "Socket was not able to be closed." << std::endl;
	}
}

bool Client::RequestFile(const std::string & fileName)
{
	if (m_file.m_transferInProgress)
	{
		std::cout << "ERROR: Function(Client::RequestFile) - File Transfer already in progress.\n";
		return false;
	}
	m_file.m_transferInProgress = true;
	m_file.m_outfileStream.open(fileName, std::ios::binary); //open file to write file to
	m_file.m_fileName = fileName; //save file name
	m_file.m_bytesWritten = 0; //reset byteswritten to 0 since we are working with a new file
	if (!m_file.m_outfileStream.is_open()) //if file failed to open...
	{
		std::cout << "ERROR: Function(Client::RequestFile) - Unable to open file: " << fileName << " for writing.\n";
		return false;
	}
	std::cout << "Requesting file from server: " << fileName << std::endl;
	Packet requestFilePacket;
	requestFilePacket.Append(PacketType::FileTransferRequestFile);
	requestFilePacket.Append(m_file.m_fileName.size());
	requestFilePacket.Append(m_file.m_fileName.c_str(), m_file.m_fileName.size());
	m_pm.Append(std::make_shared<Packet>(requestFilePacket));
	return true;
}

void Client::requestToMove(int t_x, int t_y)
{
	std::string message{ "" };
	message += static_cast<char>(t_x);
	message += static_cast<char>(t_y);
	sendString(m_pm, PacketType::RequestToMove, message);
}

void Client::setPacketRecievedCallback(PacketRecievedCallback t_callback)
{
	m_packetRecievedCallback = t_callback;
}

void Client::PacketSenderThread(Client & client) //Thread for all outgoing packets
{
	while (true)
	{
		if (client.m_terminateThreads == true)
			break;
		while (client.m_pm.HasPendingPackets())
		{
			std::shared_ptr<Packet> p = client.m_pm.Retrieve();
			if (!client.sendAll(client.m_connection, (const char*)(&p->m_buffer[0]), p->m_buffer.size()))
			{
				std::cout << "Failed to send packet to server..." << std::endl;
				break;
			}
		}
		Sleep(5);
	}
	std::cout << "Packet thread closing..." << std::endl;
}

void Client::Disconnect()
{
	m_pm.Clear();
	closesocket(m_connection);
	std::cout << "Disconnected from server." << std::endl;
}