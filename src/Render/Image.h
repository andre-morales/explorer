#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"

class Image {
public:
	Shared<byte> data;
	unsigned int width, height;

	Image();
	Image(const char*);
	Image(const std::string&);

	void loadFile(const char*);
};