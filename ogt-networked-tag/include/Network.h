#ifndef NETWORK_H
#define NETWORK_H

#include <WinSock2.h>
#include <string>
#include <functional>

#include "Network.h"
#include "PacketManager.h"

class Network
{
public:

	const char m_UNASSIGNED_ID = 100;

	using PacketRecievedCallback = std::function<void(PacketType, std::string)>;
	void setPacketRecievedCallback(PacketRecievedCallback t_callback);

	virtual char getLocalId() const = 0;

protected:

	bool sendAll(SOCKET t_socket, char const * t_data, int const t_totalBytes);
	bool recieveAll(SOCKET t_socket, char * t_data, int const t_totalBytes);
	bool getInt32_t(SOCKET t_socket, std::int32_t & t_int32_t);
	bool getPacketType(SOCKET t_socket, PacketType &  t_packetType);
	void sendString(PacketManager & t_packageManager, PacketType t_packetType, std::string const & t_string);
	bool getString(SOCKET t_socket, std::string & t_string);

	PacketRecievedCallback m_packetRecievedCallback;

private:


};

#endif // !NETWORK_H