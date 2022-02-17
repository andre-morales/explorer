#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class ZL_DecompressorStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	ZL_DecompressorStream(uint32, uint32);
	~ZL_DecompressorStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 toWrite() override;
	uint32 toRead() override;
	uint32 writeFrom(Stream*, uint32) override;
	class ZL_Decompressor* inflate_;
};

