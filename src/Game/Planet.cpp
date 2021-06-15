#include "Planet.h"
#include "Game/Chunk.h"

Planet::Planet(Universe* uni) : universe(uni), terrainGen(1){
}
Planet::~Planet(){
}

Chunk* Planet::allocateChunk(int32 cx, int32 cy, int32 cz){
	Chunk* chunk = new Chunk(this, cx, 0, cz);
	chunkMap.emplace(chunk->id, chunk);
	chunkSet.insert(chunk);
	return chunk;
}

Chunk* Planet::getChunk(int32 sx, int32 sy, int32 sz){
	auto chunk = chunkMap.find(((sy * 16384) + sz) * 16384 + sx);
	if(chunk != chunkMap.end()){
		return chunk->second;
	}
	return nullptr;
}
