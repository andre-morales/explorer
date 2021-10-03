#include "Chunk.h"
#include "Explorer/Explorer.h"
#include "Render/Renderer.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Block.h"
#include "Game/BlockInfo.h"
#include "ilib/Math/vec3.h"
#include "Exception.h"
#include "ilib/logging.h"
#include <cstring>
#ifdef EXPLORER_CLIENT
#include "Explorer/ChunkBatcher.h"
#include "Game/ChunkImpl.h"
#endif

Chunk::Chunk(Planet* pl, uint64 id) : planet(pl), batchingOpQueue(0){
	blocks = nullptr;
	Chunk::XYZ(id, &cx, &cy, &cz);
	this->id = id;
}

Chunk::Chunk(Planet* pl, int32 x, int32 y, int32 z) : Chunk(pl, Chunk::ID(x, y, z)){
}
Chunk::~Chunk(){
	free(blocks);
	blocks = nullptr;
	#ifdef EXPLORER_CLIENT
	free(batchedVerts);
	batchedVerts = nullptr;
	free(batchedUVs);
	batchedUVs = nullptr;
	free(batchedColors);
	batchedColors = nullptr;
	#endif
}

void Chunk::allocateBlocks(){
	blocks = (ChunkData*)malloc(24 * 24 * 24 * sizeof(Block));
}

#ifdef EXPLORER_SERVER
void Chunk::gen(){
	allocateBlocks();
	memset(blocks, 1, 24 * 24 * 24 * sizeof(Block));

	for(int x = 0; x < 24; x++){
		for(int z = 0; z < 24; z++){
			(*blocks)[x][0][z] = Block{2};
		}
	}
	//(*blocks)[x][n][z] = Block{3};
	//state = State::TERRAIN;
	for(int x = 0; x < 24; x++){
		for(int z = 0; z < 24; z++){
			int bx = x + (cx * 24);
			int bz = z + (cz * 24);

			double np = 32;
			double sc = 0.039196174894183;
			double pn = planet->terrainGen.get(np+bx*sc, np+bz*sc, 0);
			double ns = (0.7+pn) * 8;
			uint16 n;
			if(ns < 0) n = 0; else if(ns > 23) n = 23; else n = ns;
			//hmap[x][z] = n;

			(*blocks)[x][n][z] = Block{3};
			for(int y = 1; y < n; y++){
				(*blocks)[x][y][z] = Block{4};
			}
			(*blocks)[x][0][z] = Block{2};
		}
	}
}
#else
void Chunk::batch(){
	planet->chunkBatcher->batch(*this);
}
void Chunk::batchCenter(){
	planet->chunkBatcher->batchCenter(*this);
}
void Chunk::batchCorners(){
	planet->chunkBatcher->batchCorners(*this);
}
bool Chunk::isDeletable(){
	return !beingUsedForBatching && !batching && batchingOpQueue == 0;
}
#endif

uint64 Chunk::ID(int32 sx, int32 sy, int32 sz){
	uint64 y = sy & 0b00000000000000000000000000001111;
	uint64 z = sz & 0b00111111111111111111111111111111;
	uint64 x = sx & 0b00111111111111111111111111111111;
	return (y << 60) | (z << 30) | x;
}

void Chunk::XYZ(uint64 id, int32* sx, int32* sy, int32* sz){
	int32 y = (id & 0xF000000000000000) >> 60;
	int32 E4 = 1 << 3;
	*sy = (y ^ E4) - E4;

	int32 z = (id & 0x0FFFFFFFC0000000) >> 30;
	int32 E30 = 1 << 29;
	*sz = (z ^ E30) - E30;

	int32 x = id & 0x000000003FFFFFFF;
	*sx = (x ^ E30) - E30;
}
