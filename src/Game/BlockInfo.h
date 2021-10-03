#pragma once
#include <string>
#include "ilib/types.h"

class BlockInfo {
public:
    std::string name; // Block name.
	byte id;          // Numeric id.
	float opaqueness; // How much shadow it casts on its sides.
	bool opaque;      // Hides other blocks if true.
	byte topTexId;    // Top texture.
	byte sidesTexId;  // Side texture.
	byte bottomTexId; // Bottom texture.

	BlockInfo();
	BlockInfo(std::string, byte, float, byte);
	BlockInfo(std::string, byte, float, byte, byte, byte);
};
