#pragma once
#include "ilib/types.h"
#include "Net/PacketCodes.h"

class Socket;
class Stream;

class Packet {
public:
	unsigned short opcode;
	unsigned int length;
	byte* data;

	Packet();
	~Packet();

	void op(PacketCode);
	void send(Socket*);
	void send(Stream*);
	void get(Stream*);
};
