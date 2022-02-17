#include "BZ2_Compressor.h"
#include "ilib/IO/CircularBuffer.h"
#include "bzip2/bzlib.h"
#include "Exception.h"

BZ2_Compressor::BZ2_Compressor(){
	handle = new bz_stream();
	auto& stream = *(bz_stream*)handle;

	stream.bzalloc = nullptr;
	stream.bzfree = nullptr;
	stream.opaque = nullptr;
	int ret = BZ2_bzCompressInit(&stream, 9, 1, 250);
	if(ret != BZ_OK) {
		throw Exception("bzip2 deflate init failed.");
	}
}

void BZ2_Compressor::compress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written) {
	if(!inLen) return;
	
	auto& stream = *(bz_stream*)handle;
	stream.next_in = (char*)in;
	stream.avail_in = inLen;
	stream.next_out = (char*)out;
	stream.avail_out = outLen;
	
	int ret = BZ2_bzCompress(&stream, BZ_RUN);
	//if(ret < 0) throw Exception("BZ2 Compress failed with " + std::to_string(ret));
	read = inLen - stream.avail_in;
	written = outLen - stream.avail_out;
}

void BZ2_Compressor::flush_sync(byte* out, uint32 outLen, uint32& written) {
	written = 0;

	auto& stream = *(bz_stream*)handle;
	stream.next_in = (char*)0xFAFAFA;
	stream.avail_in = 0;

	stream.next_out = (char*)out;
	stream.avail_out = outLen;

	int ret = BZ2_bzCompress(&stream, BZ_FLUSH);
	if(ret < 0) throw Exception("BZ2 Flush failed with " + std::to_string(ret));
	written = outLen - stream.avail_out;	
}