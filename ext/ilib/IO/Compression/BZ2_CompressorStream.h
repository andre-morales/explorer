#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class BZ2_CompressorStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	BZ2_CompressorStream(uint32, uint32);
	~BZ2_CompressorStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 readInto(Stream*, uint32) override;
	uint32 toWrite() override;

	void flush_sync();

	class BZ2_Compressor* deflate_;
};