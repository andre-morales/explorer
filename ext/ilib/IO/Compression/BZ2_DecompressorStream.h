#pragma once
#include "ilib/IO/Stream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/mem.h"

class BZ2_DecompressorStream : public Stream {
public:
	un<CircularBuffer> inBuffer, outBuffer;

	BZ2_DecompressorStream(uint32, uint32);
	~BZ2_DecompressorStream();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 toWrite() override;
	uint32 toRead() override;
	uint32 writeFrom(Stream*, uint32) override;
	class BZ2_Decompressor* inflate_;
};