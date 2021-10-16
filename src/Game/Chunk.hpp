#include "Chunk.h"
#include "Explorer/Explorer.h"
#include "Render/Renderer.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Block.h"
#include "Game/BlockInfo.h"
#include "ilib/Math/vec3.h"
#include "ilib/logging.h"
#include "Exception.h"
#include <cstring>

Chunk::Chunk(Planet* pl, int32 x, int32 y, int32 z) : Chunk(pl, Chunk::ID(x, y, z)){
}

void Chunk::allocateBlocks(bool empty){
	if(blocks) throw Exception("Blocks already allocated!");
	if(empty) {
		blocks = (ChunkData*) new Block[24 * 24 * 24]();
	} else {
		blocks = (ChunkData*) new Block[24 * 24 * 24];
	}
}

//yyyy_zzzzzzzzzzzzzzzzzzzzzzzzzzzzzz_xxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
uint64 Chunk::ID(int32 sx, int32 sy, int32 sz){
	uint64 y = sy & 0b00000000000000000000000000001111;
	uint64 z = sz & 0b00111111111111111111111111111111;
	uint64 x = sx & 0b00111111111111111111111111111111;
	return (y << 60) | (z << 30) | x;
}

void Chunk::XYZ(uint64 id, int32* sx, int32* sy, int32* sz){
	int32 E4 = 1 << 3;
	int32 E30 = 1 << 29;

	int32 y = (int32)((id & 0xF000000000000000) >> 60);
	*sy = (y ^ E4) - E4;

	int32 z = (int32)((id & 0x0FFFFFFFC0000000) >> 30);
	*sz = (z ^ E30) - E30;

	int32 x = (int32)((id & 0x000000003FFFFFFF) >> 0);
	*sx = (x ^ E30) - E30;
}
