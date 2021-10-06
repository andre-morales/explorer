#pragma once

struct PacketCode {
	typedef const unsigned short int Code;

	static Code EMPTY      = 0; // Invalid opcode
	static Code HANDSHAKE  = 1; // Unusued
	static Code JOIN       = 2; // Sent by the client to the server when joining
	static Code CHUNK      = 3; // Client: means a chunk data request, Server: means a chunk delivery
	static Code CHAT_MSG   = 4; // Client: sent chat message, Server: chat text
	static Code DISCONNECT = 5; // Client: quit from server.
};
