#include "Inflate.h"
#include "zlib/zlib.h"
#include "Exception.h"

Inflate::Inflate(){
	handle = new z_stream();
	auto& stream = *handle;

	stream.zalloc = Z_NULL;
	stream.zfree = Z_NULL;
	stream.opaque = Z_NULL;
	stream.avail_in = 0;
	stream.next_in = Z_NULL;
	int ret = inflateInit(&stream);

	if(ret != Z_OK) {
		throw Exception("s");
	}
}

void Inflate::decompress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written){
	auto& stream = *handle;
	stream.avail_in = inLen;
	stream.next_in = in;

	stream.avail_out = outLen;
	stream.next_out =out;
	int ret = inflate(&stream, Z_NO_FLUSH);
	//int ret = inflate(&stream, Z_SYNC_FLUSH);
	if(ret < 0) {
		switch(ret) {
			case Z_BUF_ERROR: break;
			default:
				throw Exception("Inflate err : " + std::to_string(ret));
		}
	}
	read = inLen - stream.avail_in;
	written = outLen - stream.avail_out;
}
