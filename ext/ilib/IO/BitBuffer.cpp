#include "BitBuffer.h"

BitBuffer::BitBuffer(byte* bf, uint32 s)
: buffer(bf), bufferSize(s){}

void BitBuffer::write(byte in, uint8 bits) {
	uint8 fb = 8 - bitWriteHead; // Bits that fit into current byte.

	// If whole input fits into the current byte.
	switch (writeAlign){
	case BitGroupAlignment::LEFT_TO_RIGHT:
		if (bitWriteHead + bits <= 8) {
			bitWriteHead += bits;
			buffer[byteWriteHead] |=  in << (8 - bitWriteHead);
		} else {
			buffer[byteWriteHead++] |= in >> (bits - fb);
			buffer[byteWriteHead]   |= in << (fb - bits + 8);
			bitWriteHead = bits - fb;
		}
		break;
	case BitGroupAlignment::RIGHT_TO_LEFT:
		if (bitWriteHead + bits <= 8) {
			buffer[byteWriteHead] |= in << bitWriteHead;
			bitWriteHead += bits;
		} else {
			buffer[byteWriteHead++] |= in << bitWriteHead;
			buffer[byteWriteHead] |= in >> fb;
			bitWriteHead = bits - fb;
		}
		break;
	}

	if (bitWriteHead == 8) {
		++byteWriteHead;
		bitWriteHead = 0;
	}
}

byte BitBuffer::read(byte bits) {
	auto value = 0;
	auto fb = 8 - bitReadHead; // Bits that can be read from current byte.
	if(bits <= fb) {
		auto LBITS = 8 - bits;
		value |= (buffer[byteReadHead] >> (fb - bits)) & (0xFF >> LBITS);
		bitReadHead += bits;
	} else {
		value |= (buffer[byteReadHead] & (0xFF >> (8 - fb))) << (bits - fb);
		++byteReadHead;

		value |= buffer[byteReadHead] >> (8 - bits + fb);
		bitReadHead = bits - fb;
	}

	if(bitReadHead == 8){
		++byteReadHead;
		bitReadHead = 0;
	}

	return value;
}