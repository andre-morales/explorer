#pragma once
#include "ilib/types.h"

class Deflate {
public :

	Deflate();

	void compress(byte* in, uint32 inLen, uint32& read, byte* out, uint32 outLen, uint32& written);

private:
	struct z_stream_s* handle;
};

