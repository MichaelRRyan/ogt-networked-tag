#ifndef PACKET_MANAGER_H
#define PACKET_MANAGER_H

#include <queue> //for std::queue
#include <mutex> //for std::mutex
#include <memory> //for std::shared_ptr

#include "Packet.h" //for Packet

class PacketManager
{
private:
	std::queue<std::shared_ptr<Packet>> m_queue_packets;
	std::mutex m_mutex_packets;
public:
	void Clear();
	bool HasPendingPackets();
	void Append(std::shared_ptr<Packet> p);
	std::shared_ptr<Packet> Retrieve();
};

#endif // !PACKET_MANAGER_H