#pragma once
#include "ilib/types.h"

class PacketReader {
public:
	byte headerBuffer[4];

	bool receive(class Stream*);
	uint8 packetCode();
	uint32 packetLength();
	void markAsRead();

private:
	bool packetAvailable = false;
	byte headerBufferPtr = 0;

	
};

