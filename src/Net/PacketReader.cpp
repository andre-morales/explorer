#include "PacketReader.h"
#include "ilib/IO/Stream.h"

bool PacketReader::receive(class Stream* str) {
	if (packetAvailable) return true;

	byte* ptr = headerBuffer + headerBufferPtr;
	while (true) {
		if (str->read(ptr, 1) == 0) return false;

		if (headerBufferPtr++ >= 1) {
			// If this is the last length byte
			if ((*ptr & 0x80) == 1) {
				packetAvailable = true;
				headerBufferPtr = 0;
				return true;
			}
		}
		
		ptr++;
	}
}

uint8 PacketReader::packetCode() {
	return *headerBuffer;
}

#include "ilib/IO/BitBuffer.h"

uint32 PacketReader::packetLength() {
	byte* ptr = headerBuffer + 1;
	uint32 len = 0;
	
	

	while (true) {
		byte n = *ptr;
		
		ptr++;
	}
}

void PacketReader::markAsRead() {
	packetAvailable = false;
}