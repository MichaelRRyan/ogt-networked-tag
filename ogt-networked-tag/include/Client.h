#pragma once
#include <WinSock2.h> //For SOCKET
#include <string> //For std::string
#include <functional>

#include "FileTransferData.h" //For FileTransferData class
#include "PacketManager.h" //For PacketManager class
#include "Network.h"

class Client : public Network
{
public: // Public functions

	using PacketRecievedCallback = std::function<void(PacketType, std::string)>;

	Client(const char * ip, const int port);
	bool Connect();
	void Disconnect();
	bool RequestFile(const std::string & fileName);
	void requestToMove(int t_x, int t_y);

	void setPacketRecievedCallback(PacketRecievedCallback t_callback);

	~Client();

private: //Private functions

	bool CloseConnection();
	bool ProcessPacketType(const PacketType packetType);
	static void ClientThread(Client & client); //Client thread which reads messages from server
	static void PacketSenderThread(Client & client); //Packet sender thread which sends out packets existing in packet manager

private: //Private variables

	bool m_terminateThreads = false;
	bool m_isConnected = false;
	FileTransferData m_file; //Object that contains information about our file that is being received from the server.
	SOCKET m_connection;//This client's connection to the server
	SOCKADDR_IN m_addr; //Address to be binded to our Connection socket
	PacketManager m_pm; //Packet manager which sends all of client's outgoing packets

	std::thread m_pst; //Create thread to send packets
	std::thread m_ct; //Create thread to listen to server

	PacketRecievedCallback m_packetRecievedCallback;

};