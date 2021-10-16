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
			uint16 n;
			if(ns < 0) n = 0; else if(ns > 23) n = 23; else n = ns;

			(*blocks)[n][x][z] = Block{2};
			for(int y = 1; y < n; y++){
				(*blocks)[y][x][z] = Block{3};
			}
			(*blocks)[0][x][z] = Block{1};
		}
	}
}
#include "Game/Chunk.hpp"