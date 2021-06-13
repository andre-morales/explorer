#pragma once
#include "ilib/mem.h"
#include "Game/Block.h"
#include "Game/Planet.h"

class Chunk {
public:
    typedef Block ChunkData[24][24][24];

    Planet* planet;
    ChunkData* blocks;
    uint64 id;
    int32 cx, cy, cz;

    uint32 allocatedVerts;
    uint32 batchedVertsLen;
    float* batchedVerts;
    float* genVertsP;

    float* batchedUVs;
    float* genUVsP;

	Chunk(Planet*, int32, int32, int32);
	~Chunk();

	void addVertexAlloc();
	void addVertexAlloc(int);
	void gen();
	void batch();
private:


    bool sNotAir(int32, int32, int32);
	bool sOpaque(int32, int32, int32);
	float sOpaqueness(int32, int32, int32);
    void addFace(byte, byte, byte, byte, byte);
};
