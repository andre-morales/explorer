#pragma once
#include "ilib/types.h"
#include "ilib/mem.h"
#include <map>
#include <set>
class Chunk;
class Universe;

class Planet {
public:
    Universe* universe;

    std::set<Chunk*> chunkSet;
    std::map<uint64, Chunk*> chunkMap;

	Planet(Universe*);
	~Planet();
};
