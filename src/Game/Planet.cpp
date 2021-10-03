#include "Planet.h"
#include "Game/Chunk.h"
#include <iostream>
#ifdef EXPLORER_CLIENT
#include "Explorer/ChunkBatcher.h"
#endif
Planet::Planet(Universe* uni) : universe(uni), terrainGen(1){
	#ifdef EXPLORER_CLIENT
	chunkBatcher.reset(new ChunkBatcher(*this));
	#endif
}
Planet::~Planet(){
	for(Chunk* ch : chunkSet){
		delete ch;
	}
}

Chunk* Planet::createChunk(int32 cx, int32 cy, int32 cz){
	return createChunk(Chunk::ID(cx, cy, cz));
}

Chunk* Planet::createChunk(uint64 id){
	Chunk* chunk = new Chunk(this, id);
	chunkMap.emplace(chunk->id, chunk);
	chunkSet.insert(chunk);
	return chunk;
}

Chunk* Planet::getChunk(int32 cx, int32 cy, int32 cz){
	return getChunk(Chunk::ID(cx, cy, cz));
}

Chunk* Planet::getChunk(uint64 id){
	auto chunk = chunkMap.find(id);
	if(chunk != chunkMap.end()){
		return chunk->second;
	}
	return nullptr;
}

void Planet::getChunkNeighbours(const Chunk& c, Chunk*& l, Chunk*& r, Chunk*& f, Chunk*& b){
	l = getChunk(c.cx - 1, c.cy, c.cz);
	r = getChunk(c.cx + 1, c.cy, c.cz);
	f = getChunk(c.cx, c.cy, c.cz + 1);
	b = getChunk(c.cx, c.cy, c.cz - 1);
};
void Planet::getChunkNeighbours8(const Chunk& c, Chunk** nb){
	nb[0] = getChunk(c.cx - 1, c.cy, c.cz);
	nb[1] = getChunk(c.cx - 1, c.cy, c.cz + 1);
	nb[2] = getChunk(c.cx    , c.cy, c.cz + 1);
	nb[3] = getChunk(c.cx + 1, c.cy, c.cz + 1);
	nb[4] = getChunk(c.cx + 1, c.cy, c.cz);
	nb[5] = getChunk(c.cx + 1, c.cy, c.cz - 1);
	nb[6] = getChunk(c.cx    , c.cy, c.cz - 1);
	nb[7] = getChunk(c.cx - 1, c.cy, c.cz - 1);
};
void Planet::deleteChunk(int32 sx, int32 sy, int32 sz){
	deleteChunk(Chunk::ID(sx, sy, sz));
}
void Planet::deleteChunk(uint64 id){
	auto it = chunkMap.find(id);
	Chunk* ch = it->second;
	//ch->deleted = true;
	chunkSet.erase(ch);
	chunkMap.erase(it);
	delete ch;
}
