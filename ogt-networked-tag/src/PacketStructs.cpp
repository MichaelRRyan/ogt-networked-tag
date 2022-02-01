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

	PlayerJoined::PlayerJoined(char t_id, int t_x, int t_y) :
		m_id{ t_id },
		m_x{ t_x },
		m_y{ t_y }
	{
	}

	std::shared_ptr<Packet> PlayerJoined::toPacket()
	{
		std::shared_ptr<Packet> p = std::make_shared<Packet>();
		p->Append(PacketType::PlayerJoined);

		std::string info{ "" };
		info += m_id; // ID
		info += static_cast<char>(m_x); // Tile x
		info += static_cast<char>(m_y); // Tile y

		p->Append(info.size());
		p->Append(info);
		return p;
	}
}