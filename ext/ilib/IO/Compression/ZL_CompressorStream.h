#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class ZL_CompressorStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	ZL_CompressorStream(uint32, uint32);
	~ZL_CompressorStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 readInto(Stream*, uint32) override;
	uint32 toWrite() override;
	uint32 toRead() override;
	void flush_sync();

	class ZL_Compressor* deflate_;
};