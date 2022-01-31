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
	RockMoved,

	ChatMessage,
	FileTransferRequestFile, //Sent to request a file
	FileTransfer_EndOfFile, //Sent for when file transfer is complete
	FileTransferByteBuffer, //Sent before sending a byte buffer for file transfer
	FileTransferRequestNextBuffer //Sent to request the next buffer for file
};

#endif // !PACKET_TYPE_H