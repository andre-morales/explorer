#pragma once
#include "ilib/types.h"
#include "ilib/mem.h"
#include "ilib/math/noise.h"
#include "ilib/obs_unordered_map.h"
#include <unordered_map>
#include <set>
class Chunk;
class ChunkBatcher;
class Universe;

class Planet {
public:
	Universe* universe;
	float dayTime;
	float sunXRotation = 30;
	Noise terrainGen;
	std::set<Chunk*> chunkSet;
	obs_unordered_map<uint64, Chunk*> chunkMap;
	#ifdef EXPLORER_CLIENT
	Unique<ChunkBatcher> chunkBatcher;
	#endif

	Planet(Universe*);
	~Planet();

	Chunk* createChunk(uint64);
	Chunk* createChunk(int32, int32, int32);
	Chunk* getChunk(uint64);
	Chunk* getChunk(int32, int32, int32);
	void getChunkNeighbours(const Chunk&, Chunk*&, Chunk*&, Chunk*&, Chunk*&);
	void getChunkNeighbours8(const Chunk&, Chunk**);
	void deleteChunk(uint64);
	void deleteChunk(int32, int32, int32);
};
