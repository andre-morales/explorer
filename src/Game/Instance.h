#pragma once
#include "Game/BlockInfo.h"
#include "ilib/mem.h"
#include <vector>

class Explorer;
class Universe;

class Instance {
public:
	Explorer* explorer;

	std::vector<BlockInfo> registeredBlocks;
	std::vector<Shared<Universe>> universes;

	Instance();
	~Instance();

	void registerBlock(byte);
};
