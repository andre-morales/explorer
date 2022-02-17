#include "ServerChunk.h"
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
#include "ilib/Math/maths.h"
#include <cstring>

Chunk::Chunk(Planet* pl, uint64 id) : blocks(nullptr), id(id), planet(pl){
	Chunk::XYZ(id, &cx, &cy, &cz);
}
Chunk::~Chunk(){
	delete[] blocks;
}

void Chunk::gen(){
	if(!blocks) throw Exception("Blocks not allocated!");

	for(int x = 0; x < 24; x++){
		for(int z = 0; z < 24; z++){
			int bx = x + (cx * 24);
			int bz = z + (cz * 24);

			double np = 32;
			double sc = 0.039196174894183;
			double pn = planet->terrainGen.get(np+bx*sc, np+bz*sc, 0);
			double ns = (0.7+pn) * 8;

			uint16 n = Math::clamp(ns, 0.0, 23.0);

			if(n + 1 > highestBlockY) {
				highestBlockY = n + 1;
			}

			(*blocks)[n][x][z] = Block{2};
			for(int y = 1; y < n; y++){
				(*blocks)[y][x][z] = Block{3};
			}
			(*blocks)[0][x][z] = Block{1};
		}
	}
}
#include "ilib/IO/BitBuffer.h"
BitBuffer* Chunk::toBitfield () {
	BitBuffer* bf = new BitBuffer();
	bf->bufferSize = highestBlockY * 5 * 24 * 24 / 8;
	bf->buffer = new byte[bf->bufferSize]();
	
	for(int y = 0; y < highestBlockY; y++){
		for(int x = 0; x < 24; x++){
			for(int z = 0; z < 24; z++){
				bf->write((*blocks)[y][x][z].id, 5);
			}
		}
	}

	return bf;
}

void Chunk::fromBitfield(BitBuffer* bf) {
	/*for (int y = 0; y < bf->bufferSize / 24 / 24; y++) {
		for (int x = 0; x < 24; x++) {
			for (int z = 0; z < 24; z++) {
				byte id = bf->read(8);
				(*blocks)[y][x][z].id = id;
			}
		}
	}*/
}

BitBuffer* Chunk::toBitfield_() {
	BitBuffer* bf = new BitBuffer();
	bf->bufferSize = highestBlockY * 7 * 24 * 24 / 8;
	bf->buffer = new byte[bf->bufferSize]();

	for (int y = 0; y < highestBlockY; y++) {
		for (int x = 0; x < 24; x++) {
			for (int z = 0; z < 24; z++) {
				bf->write((*blocks)[y][x][z].id, 7);
			}
		}
	}

	return bf;
}

void Chunk::fromBitfield_(BitBuffer* bf) {
	for (uint32 y = 0; y < bf->bufferSize * 8 / 24 / 24 / 7; y++) {
		for (uint32 x = 0; x < 24; x++) {
			for (uint32 z = 0; z < 24; z++) {
				byte id = bf->read(7);
				(*blocks)[y][x][z].id = id;
			}
		}
	}
}
#include "Game/Chunk.hpp"