#include "ClientChunk.h"
#include "Explorer/Explorer.h"
#include "Explorer/ChunkBatcher.h"
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

Chunk::Chunk(Planet* pl, uint64 i) : id(i), planet(pl), batchingOpQueue(0){
	Chunk::XYZ(id, &cx, &cy, &cz);
}

Chunk::~Chunk(){
	delete[] blocks;
	free(batchedVerts);
	free(batchedUVs);
	free(batchedColors);
	free(batchedNormals);
}

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

#include "ilib/IO/BitBuffer.h"
void Chunk::fromBitfield(BitBuffer* bf) {
	//bf->bufferSize = highestBlockY * 24 * 24 / 4;
	//bf->buffer = new byte[bf->bufferSize]();

	for(int y = 0; y < bf->bufferSize * 8 / 24 / 24 / 5; y++){
		for(int x = 0; x < 24; x++){
			for(int z = 0; z < 24; z++){
				byte id = bf->read(5);
				(*blocks)[y][x][z].id = id;
			}
		}
	}
}

#include "Game/Chunk.hpp"