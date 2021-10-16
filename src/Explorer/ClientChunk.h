#pragma once
#include "ilib/mem.h"
#include "Game/Block.h"
#include "Game/Planet.h"
#include <mutex>

class Chunk {
public:
	//                      Y   X   Z
	typedef Block ChunkData[24][24][24];

	enum class State {
		EMPTY, REQUESTED, FINISHED
	} state = State::EMPTY;
	std::mutex stateLock;
	bool deleteFlag = false;
	std::atomic<int> batchingOpQueue;
	bool batching = false;
	bool beingUsedForBatching = false;

	bool batchedCenter = false;
	bool batchedCorners = false;

	uint32 vbo = 0;

	uint32 allocatedVerts = 0;  // Max allocated vertices
	uint32 batchedVertsLen = 0; // Actual used vertices
	uint32 cornerVertsLen = 0;
	float* batchedVerts = 0; // Full vertex array
	float* cornerVertsP = 0; // Start of corner vertices
	float* genVertsP = 0;    // Vertex write head for batching

	float* batchedUVs = 0;  // Full UV array
	float* cornerUVsP = 0;  // Corner UVs array
	float* genUVsP = 0;     // UV write head

	byte* batchedColors = 0; // Color array
	byte* cornerColorsP = 0; // Start of corner colors
	byte* genColorsP = 0;    // Color write head

	int8* batchedNormals = 0;
	int8* cornerNormalsP = 0;
	int8* genNormalsP = 0;

	Planet* planet;
	ChunkData* blocks = 0;
	uint64 id;
	int32 cx, cy, cz;

	Chunk(Planet*, uint64);
	Chunk(Planet*, int32, int32, int32);
	~Chunk();

	void allocateBlocks(bool);
	void batch();
	void batchCenter();
	void batchCorners();
	bool isDeletable();

	static uint64 ID(int32, int32, int32);
	static void XYZ(uint64, int32*, int32*, int32*);	

private:
	Chunk(const Chunk&) = delete;
};
