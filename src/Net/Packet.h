#pragma once
#include "ilib/types.h"

class Stream;

class Packet {
public:
	unsigned short opcode;
	unsigned int length;
	byte* data;

	Packet();
	~Packet();

	void send(Stream&);
	void get(Stream&);
};
