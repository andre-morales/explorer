#pragma once
#include "ilib/types.h"

class Inflate {
public :
	Inflate();

	void decompress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written);
private:
	struct z_stream_s* handle;
};

