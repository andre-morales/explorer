#pragma once
#include "ilib/types.h"

class ZL_Compressor {
public :

	ZL_Compressor();

	void compress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written);
	void flush_sync(byte* out, uint32 outLen, uint32& written);

private:
	void* handle;
};