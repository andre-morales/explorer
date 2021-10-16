#include "Deflate.h"
#include "zlib/zlib.h"
#include "ilib/IO/CircularBuffer.h"
#include "Exception.h"

Deflate::Deflate(){
	handle = new z_stream();
	auto& stream = *handle;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	int ret = deflateInit(&stream, 9);
	if(ret != Z_OK) {
		throw Exception("zlib deflate init failed.");
	}
}

void Deflate::compress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written) {
	auto& stream = *handle;
	stream.next_in = in;
	stream.avail_in = inLen;

	stream.next_out = out;
	stream.avail_out = outLen;
	int ret = deflate(&stream, Z_NO_FLUSH);

	read = inLen - stream.avail_in;
	written = outLen - stream.avail_out;
}