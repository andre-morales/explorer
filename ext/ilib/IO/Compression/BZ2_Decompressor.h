#pragma once
#include "ilib/types.h"

class BZ2_Decompressor {
public :
	BZ2_Decompressor();

	void decompress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written);
private:
	void* handle;
};

