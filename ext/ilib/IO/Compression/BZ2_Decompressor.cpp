#include "BZ2_Decompressor.h"
#include "bzip2/bzlib.h"
#include "Exception.h"

BZ2_Decompressor::BZ2_Decompressor(){
	handle = new bz_stream();
	auto& stream = *(bz_stream*)handle;

	stream.bzalloc = nullptr;
	stream.bzfree = nullptr;
	stream.opaque = nullptr;
	stream.avail_in = 0;
	stream.next_in = nullptr;
	int ret = BZ2_bzDecompressInit(&stream, 1, 0);

	if(ret != BZ_OK) {
		throw Exception("s");
	}
}

void BZ2_Decompressor::decompress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written){
	//if(outLen == 0) return;
	
	auto& stream = *(bz_stream*)handle;
	stream.avail_in = inLen;
	stream.next_in = (char*)in;
	
	stream.avail_out = outLen;
	stream.next_out = (char*)out;
	int ret = BZ2_bzDecompress(&stream);
	if(ret != BZ_OK) {
		throw Exception("Inflate err : " + std::to_string(ret));
	}
	read = inLen - stream.avail_in;
	written = outLen - stream.avail_out;
}