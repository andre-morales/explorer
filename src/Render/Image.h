#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include <string>

class Image {
public:
	sh<byte> data;
	uint32 width = 0, height = 0;

	Image();
	Image(const char*);
	Image(const std::string&);

	void loadFile(const char*);
};