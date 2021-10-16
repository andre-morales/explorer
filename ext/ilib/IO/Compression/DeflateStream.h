#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class DeflateStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	DeflateStream(uint32, uint32);
	~DeflateStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 toWrite() override;

	class Deflate* deflate_;
};

