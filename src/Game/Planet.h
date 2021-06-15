#pragma once
#include "ilib/types.h"
#include "ilib/mem.h"
#include "ilib/math/noise.h"
#include <map>
#include <set>
class Chunk;
class Universe;

class Planet {
public:
    Universe* universe;
	Noise terrainGen;
    std::set<Chunk*> chunkSet;
    std::map<uint64, Chunk*> chunkMap;

	Planet(Universe*);
	~Planet();

	Chunk* allocateChunk(int32, int32, int32);
	Chunk* getChunk(int32, int32, int32);
};
