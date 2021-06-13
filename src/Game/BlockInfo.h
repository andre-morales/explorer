#pragma once
#include <string>
#include "ilib/types.h"

struct BlockInfo {
    std::string name;
	byte id;
	float opaqueness;
	byte topTexId;
	byte sidesTexId;
	byte bottomTexId;
	bool opaque;

	//BlockInfo(std::string, byte, float, byte, byte, byte);
};
