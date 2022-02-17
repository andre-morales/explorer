#pragma once
#include "ilib/types.h"

class BitBuffer {
public:
	uint32 byteWriteHead = 0;
	uint8 bitWriteHead = 0;

	uint32 byteReadHead = 0;
	uint8 bitReadHead = 0;

	enum class BitGroupAlignment {
		LEFT_TO_RIGHT, RIGHT_TO_LEFT
	};

	/* This flag controls how a call to write() such as 
	write(0b00011111, 4) will use the input bits. 
	* x are ignored bits
	* 0 or 1 are actually used as input to the buffer.
	RIGHT: write(0bxxxx1111, 4)
	LEFT:  write(0b0001xxxx, 4)
	
	The default is RIGHT.*/
	//BitGroupAlignment writeInputAlign = BitGroupAlignment::RIGHT;

	/* This flag controls how a call to write() lays the bits in the
	* internal buffer.

	Examples for two consecutive write calls with input set to RIGHT.
	write(0b00001111, 4)
	write(0b00111000, 6)

	* x are not yet written bits
	* 0 or 1 are written bits
	
	LEFT:
	xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
	1111xxxx xxxxxxxx xxxxxxxx xxxxxxxx
	11111110 00xxxxxx xxxxxxxx xxxxxxxx

	RIGHT:
	xxxxxxxx xxxxxxxx xxxxxxxx xxxxxxxx
	xxxx1111 xxxxxxxx xxxxxxxx xxxxxxxx
	11101111 xxxxxx00 xxxxxxxx xxxxxxxx

	The default is LEFT.*/
	BitGroupAlignment writeAlign = BitGroupAlignment::LEFT_TO_RIGHT;

	//BitGroupAlignment readOutputAlign = BitGroupAlignment::RIGHT_TO_LEFT;
	BitGroupAlignment readAlign = BitGroupAlignment::LEFT_TO_RIGHT;

	byte* buffer = 0;
	uint32 bufferSize = 0;

	BitBuffer() = default;
	BitBuffer(byte* buffer, uint32 bufferSize);

	void write(byte input, uint8 bits);
	byte read(byte bits);
};

