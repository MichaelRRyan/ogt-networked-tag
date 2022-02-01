#include "PacketStructs.h"

namespace PS
{
	ChatMessage::ChatMessage(const std::string & msg)
		:m_message(msg)
	{
	}

	std::shared_ptr<Packet> ChatMessage::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::ReadyToPlay);
		p->Append(m_message.size());
		p->Append(m_message);
		return p;
	}
}