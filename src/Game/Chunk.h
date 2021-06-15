#pragma once
#include "ilib/mem.h"
#include "Game/Block.h"
#include "Game/Planet.h"

class Chunk {
public:
	enum class State {
		EMPTY,
		REQUESTED, RECEIVING, COMPLETE, BATCHED
	} state;

	typedef Block ChunkData[24][24][24];

	Planet* planet;
	ChunkData* blocks;
	uint64 id;
	int32 cx, cy, cz;

	#ifdef EXPLORER_CLIENT
	uint32 allocatedVerts = 0;
	uint32 batchedVertsLen = 0;
	float* batchedVerts = nullptr;
	float* genVertsP = nullptr;

	float* batchedUVs = nullptr;
	float* genUVsP = nullptr;

	byte* batchedColors = nullptr;
	byte* genColorsP = nullptr;
	#endif

	Chunk(Planet*, int32, int32, int32);
	~Chunk();

	void gen();

	#ifdef EXPLORER_CLIENT
	void addVertexAlloc();
	void addVertexAlloc(int);
	void batch();
	#endif
private:
	#ifdef EXPLORER_CLIENT
	bool sNotAir(int32, int32, int32);
	bool sOpaque(int32, int32, int32);
	float sOpaqueness(int32, int32, int32);
	void addFace(byte, byte, byte, byte, byte);
	#endif
};
