#pragma once
#include "ilib/mem.h"
#include "Game/Block.h"

class Planet;

class Chunk {
public:
	typedef Block ChunkData[24][24][24];

	enum class State {
		EMPTY, SERVER_FINISHED
	};

	State state = State::EMPTY;
	Planet* planet;
	ChunkData* blocks = 0;
	uint64 id;
	int32 cx, cy, cz;

	Chunk(Planet*, uint64);
	Chunk(Planet*, int32, int32, int32);
	~Chunk();

	void allocateBlocks();
	void gen();

	static uint64 ID(int32, int32, int32);
	static void XYZ(uint64, int32*, int32*, int32*);	

private:
	Chunk(const Chunk&) = delete;
};
