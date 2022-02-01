#ifndef PACKET_STRUCTS_H
#define PACKET_STRUCTS_H

#include <string> //for std::string
#include <memory> //for std::shared_ptr

#include "PacketType.h"
#include "Packet.h"

namespace PS //Packet Structures Namespace
{
	class ChatMessage
	{
	public:
		ChatMessage(const std::string & str);
		std::shared_ptr<Packet> toPacket(); //Converts ChatMessage to packet
	private:
		std::string m_message;
	};
}

#endif // !PACKET_STRUCTS_H