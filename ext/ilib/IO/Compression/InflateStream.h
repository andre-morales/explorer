#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class InflateStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	InflateStream(uint32, uint32);
	~InflateStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 toWrite() override;
	uint32 toRead() override;
	uint32 writeFrom(Stream*, uint32) override;
	class Inflate* inflate_;
};

