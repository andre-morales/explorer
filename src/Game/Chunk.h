#pragma once
#include "ilib/mem.h"
#include "Game/Block.h"
#include "Game/Planet.h"
#include <mutex>

class Chunk {
public:
	typedef Block ChunkData[24][24][24];

	#ifdef EXPLORER_CLIENT
		enum class State {
			EMPTY, REQUESTED, FINISHED
		};
		std::mutex stateLock;
		bool deleteFlag = false;
		std::atomic<int> batchingOpQueue;
		bool batching = false;
		bool beingUsedForBatching = false;

		bool batchedCenter = false;
		bool batchedCorners = false;

		uint32 allocatedVerts = 0;  // Max allocated vertices
		uint32 batchedVertsLen = 0; // Actual used vertices
		uint32 cornerVertsLen = 0;
		float* batchedVerts = nullptr; // Full vertex array
		float* cornerVertsP = nullptr; // Start of corner vertices
		float* genVertsP = nullptr;    // Vertex write head for batching

		float* batchedUVs = nullptr;  // Full UV array
		float* cornerUVsP = nullptr;  // Corner UVs array
		float* genUVsP = nullptr;     // UV write head

		byte* batchedColors = nullptr; // Color array
		byte* cornerColorsP = nullptr; // Start of corner colors
		byte* genColorsP = nullptr;    // Color write head

	#elif EXPLORER_SERVER
		enum class State {
			EMPTY, SERVER_FINISHED
		};
	#endif

	State state = State::EMPTY;
	Planet* planet;
	ChunkData* blocks = nullptr;
	uint64 id;
	int32 cx, cy, cz;

	Chunk(Planet*, uint64);
	Chunk(Planet*, int32, int32, int32);
	~Chunk();

	void allocateBlocks();
	void gen();

	#ifdef EXPLORER_CLIENT
	void batch();
	void batchCenter();
	void batchCorners();
	bool isDeletable();
	#endif

	static uint64 ID(int32, int32, int32);
	static void XYZ(uint64, int32*, int32*, int32*);	

private:
	Chunk(const Chunk&) = delete;
};
