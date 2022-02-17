#pragma once
#include "Stream.h"
#include "ilib/types.h"

class CircularBuffer : public Stream {
public:
	uint32 bufferSize = 0;
	byte* buffer = 0;

	uint32 writeHead = 0;
	uint32 readHead = 0;
	bool full = false;

	CircularBuffer();
	CircularBuffer(uint32);
	~CircularBuffer();

	uint32 write(const byte*, uint32) override;
	uint32 writeFrom(Stream*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 readInto(Stream*, uint32) override;
	uint32 toRead() override;
	uint32 toWrite() override;
	
	void fixWriteHead();
	void fixReadHead();
	uint32 toWrite_infront();
	uint32 toRead_infront();
	void validateHeads();
private:
	inline void dbg_validate();
};

