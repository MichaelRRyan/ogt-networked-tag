#include "Network.h"

////////////////////////////////////////////////////////////////////////////////
void Network::setPacketRecievedCallback(PacketRecievedCallback t_callback)
{
	m_packetRecievedCallback = t_callback;
}

////////////////////////////////////////////////////////////////////////////////
bool Network::sendAll(SOCKET t_socket, char const* t_data, int const t_totalBytes)
{
	int bytesSent = 0; // Holds the total bytes sent.
	while (bytesSent < t_totalBytes) // While we still have more bytes to send.
	{
		int retnCheck = send(t_socket, t_data + bytesSent, t_totalBytes - bytesSent, 0); // Try to send remaining bytes.
		if (retnCheck == SOCKET_ERROR) // If there is a socket error while trying to send bytes.
			return false; // Return false - failed to sendall.
		bytesSent += retnCheck; // Add to total bytes sent.
	}
	return true; // Success!
}

////////////////////////////////////////////////////////////////////////////////
bool Network::recieveAll(SOCKET t_socket, char * t_data, int const t_totalBytes)
{
	int bytesReceived = 0; // Holds the total bytes received.
	while (bytesReceived < t_totalBytes) // While we still have more bytes to recv.
	{
		int retnCheck = recv(t_socket, t_data + bytesReceived, t_totalBytes - bytesReceived, 0); // Try to recv remaining bytes.
		if (retnCheck == SOCKET_ERROR || retnCheck == 0) // If there is a socket error while trying to recv bytes or if connection lost.
			return false; // Return false - failed to recvall.
		bytesReceived += retnCheck; // Add to total bytes received.
	}
	return true; // Success!
}

////////////////////////////////////////////////////////////////////////////////
bool Network::getInt32_t(SOCKET t_socket, std::int32_t & t_int32_t)
{
	if (!recieveAll(t_socket, (char*)&t_int32_t, sizeof(std::int32_t))) // Try to receive long (4 byte int)... If int fails to be recv'd.
		return false; // Return false: Int not successfully received.
	t_int32_t = ntohl(t_int32_t); // Convert long from Network Byte Order to Host Byte Order.
	return true; // Return true if we were successful in retrieving the int.
}

////////////////////////////////////////////////////////////////////////////////
bool Network::getPacketType(SOCKET t_socket, PacketType & t_packetType)
{
	std::int32_t packettype_int;
	if (!getInt32_t(t_socket, packettype_int)) // Try to receive packet type... If packet type fails to be recv'd.
		return false; // Return false: packet type not successfully received.
	t_packetType = (PacketType)packettype_int;
	return true; // Return true if we were successful in retrieving the packet type.
}

////////////////////////////////////////////////////////////////////////////////
void Network::sendString(PacketManager & t_packageManager, PacketType t_packetType, std::string const& t_string)
{
	std::shared_ptr<Packet> p = std::make_shared<Packet>();
	p->Append(t_packetType);
	p->Append(t_string.size());
	p->Append(t_string);
	t_packageManager.Append(p);
}

////////////////////////////////////////////////////////////////////////////////
bool Network::getString(SOCKET t_socket, std::string& t_string)
{
	std::int32_t bufferlength; // Holds length of the message.
	if (!getInt32_t(t_socket, bufferlength)) // Get length of buffer and store it in variable: bufferlength.
		return false; // If get int fails, return false.
	if (bufferlength == 0) return true;
	t_string.resize(bufferlength); // Resize string to fit message.
	return recieveAll(t_socket, &t_string[0], bufferlength);
}

////////////////////////////////////////////////////////////////////////////////