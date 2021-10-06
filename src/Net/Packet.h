#pragma once
#include "ilib/types.h"
#include "Net/PacketCodes.h"
#include <string>

class Socket;
class Stream;

class Packet {
public:
	uint32 length = 0;
	byte* data = 0;
	uint16 opcode = 0;
	byte flags = 0;

	Packet();
	Packet(uint32, byte*, uint16, byte);
	Packet(Packet&&) noexcept;
	~Packet();

	/* Copies all the data in src given the specified length. */
	void copyFullData(const void*, uint32);

	/* When set, this packet owns its data pointer, and shall delete its data when the packet itself gets destroyed. */
	void setDeleteFlag();

	void send(Socket*) const;
	void send(Stream*) const;
	std::string asString() const;
private:
	Packet(const Packet&) = delete;
};
