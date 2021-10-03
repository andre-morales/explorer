#pragma once
#include "ilib/types.h"
#include "Net/PacketCodes.h"
#include <string>

class Socket;
class Stream;

class Packet {
public:
	uint16 opcode;
	uint32 length;
	byte* data;

	Packet();
	~Packet();

	void op(PacketCode);

	void send(Socket*) const;
	void send(Stream*) const;
	std::string asString() const;
};
