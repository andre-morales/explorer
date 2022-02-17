#include "ZL_Compressor.h"
#include "zlib/zlib.h"
#include "ilib/IO/CircularBuffer.h"
#include "Exception.h"

ZL_Compressor::ZL_Compressor(){
	handle = new z_stream();
	auto& stream = *(z_stream*)handle;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	int ret = deflateInit2(&stream, 9, Z_DEFLATED, 15, 8, Z_DEFAULT_STRATEGY);
	if(ret != Z_OK) {
		throw Exception("zlib deflate init failed.");
	}
}

void ZL_Compressor::compress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written) {
	auto& stream = *(z_stream*)handle;
	stream.next_in = in;
	stream.avail_in = inLen;

	stream.next_out = out;
	stream.avail_out = outLen;
	int ret = deflate(&stream, Z_NO_FLUSH);

	read = inLen - stream.avail_in;
	written = outLen - stream.avail_out;
}

void ZL_Compressor::flush_sync(byte* out, uint32 outLen, uint32& written) {
	auto& stream = *(z_stream*)handle;
	stream.next_in = 0;
	stream.avail_in = 0;

	stream.next_out = out;
	stream.avail_out = outLen;
	int ret = deflate(&stream, Z_SYNC_FLUSH);

	written = outLen - stream.avail_out;
}