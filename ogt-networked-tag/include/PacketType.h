#ifndef PACKET_TYPE_H
#define PACKET_TYPE_H

enum class PacketType
{
	// Client to Server.
	RequestToMove,
	ReadyToPlay,

	// Server to Client.
	GameStarted,
	GameEnded,
	JoinInfo,
	PlayerJoined,
	MovePlayer,
	PlayerDied,
	RockMoved
};

#endif // !PACKET_TYPE_H