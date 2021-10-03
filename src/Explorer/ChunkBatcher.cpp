#include "ChunkBatcher.h"
#include "Explorer/Explorer.h"
#include "Game/BlockInfo.h"
#include "Game/Planet.h"
#include "Game/Universe.h"
#include "Game/Instance.h"
#include "Render/Renderer.h"
#include "Render/TextureAtlas.h"
#include "Render/Geometry.h"
#include "ilib/Math/vec3.h"
#include "Exception.h"
ChunkBatcher::ChunkBatcher(Planet& p)
: planet(p), dataInstance(*p.universe->instance), blockDB(dataInstance.registeredBlocks){
	auto& atl = *(TextureAtlas*)dataInstance.explorer->renderer->getTexture("blocks_atlas").get();
	atlas.width = atl.width;
	atlas.height = atl.height;
	atlas.tileWidth = atl.tileWidth;
	atlas.tileHeight = atl.tileHeight;

	atlas.horizontalTiles = atl.width / atl.tileWidth;
	atlas.fTileWidth = atl.tileWidth / (float)atl.width;
	atlas.fTileHeight = atl.tileHeight / (float)atl.height;
}

void ChunkBatcher::batch(Chunk& chunk){
	batchCenter(chunk);
	batchCorners(chunk);
}

void ChunkBatcher::batchCenter(Chunk& chunk){
	chunk.batching = true;
	auto& infos = blockDB;
	auto& data = *chunk.blocks;

	// Set write heads to beginning of the allocated arrays.
	chunk.genVertsP = chunk.batchedVerts;
	chunk.genUVsP = chunk.batchedUVs;
	chunk.genColorsP = chunk.batchedColors;
	chunk.batchedVertsLen = 0;

	BatchContext bc{chunk, 0};
	for(int x = 1; x < 23; x++){
		for(int y = 0; y < 24; y++){
			for(int z = 1; z < 23; z++){
				Block& block = data[x][y][z];
				uint8 id = block.id;

				if(id > 1){
					byte topBlock = 0;
					byte bottomBlock = 0;
					byte leftBlock = 0;
					byte rightBlock = 0;
					byte frontBlock = 0;
					byte backBlock = 0;

					if(y > 0)  bottomBlock = data[x][y - 1][z].id;
					if(y < 23) topBlock = data[x][y + 1][z].id;
					leftBlock = data[x-1][y][z].id;
					rightBlock = data[x+1][y][z].id;
					frontBlock = data[x][y][z + 1].id;
					backBlock = data[x][y][z - 1].id;

					bool top = !infos[topBlock].opaque;
					bool bottom = !infos[bottomBlock].opaque;
					bool left = !infos[leftBlock].opaque;
					bool right = !infos[rightBlock].opaque;
					bool front = !infos[frontBlock].opaque;
					bool back = !infos[backBlock].opaque;
					
					if(top)    addFace(bc, id, 0, x, y, z);
					if(bottom) addFace(bc, id, 1, x, y, z);
					if(left)   addFace(bc, id, 2, x, y, z);
					if(right)  addFace(bc, id, 3, x, y, z);
					if(front)  addFace(bc, id, 4, x, y, z);
					if(back)   addFace(bc, id, 5, x, y, z);
				}
			}
		}
	}

	chunk.cornerVertsP = chunk.genVertsP;
	chunk.cornerUVsP = chunk.genUVsP;
	chunk.cornerColorsP = chunk.genColorsP;
	chunk.cornerVertsLen = chunk.batchedVertsLen;
	chunk.batchedCenter = true;
	chunk.batchedCorners = false;
	chunk.batching = false;
}


void ChunkBatcher::batchCorners(Chunk& chunk){
	if(!chunk.batchedCenter || chunk.batchedCorners) return;

	Chunk* neighbours[8];
	Chunk::ChunkData* ndata[8]{};

	chunk.planet->getChunkNeighbours8(chunk, neighbours);

	for(int32 i = 0; i < 8;) if(!neighbours[i++]) return;
	for(int32 i = 0; i < 8; i++) {
		auto& nb = *neighbours[i];
		std::lock_guard g(nb.stateLock);
		if(nb.blocks && !nb.deleteFlag){
			nb.beingUsedForBatching = true;
			ndata[i] = nb.blocks;
		}
	}
	
	Chunk::ChunkData& data = *chunk.blocks;
	BatchContext bc{chunk, neighbours};
	auto alg = [this, ndata, bc](int8 x, int8 z){
		auto& data = *bc.ch.blocks;
		for(int y = 0; y < 24; y++){
			auto& block = data[x][y][z];
			byte id = block.id;

			if(id > 1){
				auto* lb = ndata[0];
				auto* fb = ndata[2];
				auto* rb = ndata[4];
				auto* bb = ndata[6];

				byte topBlock = 0;
				byte bottomBlock = 0;
				byte leftBlock = 0;
				byte rightBlock = 0;
				byte frontBlock = 0;
				byte backBlock = 0;

				if(y > 0)  bottomBlock = data[x][y - 1][z].id;
				if(y < 23) topBlock = data[x][y + 1][z].id;

				if(x > 0)  leftBlock = data[x-1][y][z].id;
				else if(lb) leftBlock = (*lb)[23][y][z].id;

				if(x < 23) rightBlock = data[x+1][y][z].id;
				else if(rb) rightBlock = (*rb)[0][y][z].id;

				if(z < 23) frontBlock = data[x][y][z + 1].id;
				else if(fb)frontBlock = (*fb)[x][y][0].id;

				if(z > 0)  backBlock = data[x][y][z - 1].id;
				else if(bb) backBlock =(*bb)[x][y][23].id;

				bool top = !blockDB[topBlock].opaque;
				bool bottom = !blockDB[bottomBlock].opaque;
				bool left = !blockDB[leftBlock].opaque;
				bool right = !blockDB[rightBlock].opaque;
				bool front = !blockDB[frontBlock].opaque;
				bool back = !blockDB[backBlock].opaque;
				
				if(top)    addFace(bc, id, 0, x, y, z);
				if(bottom) addFace(bc, id, 1, x, y, z);
				if(left)   addFace(bc, id, 2, x, y, z);
				if(right)  addFace(bc, id, 3, x, y, z);
				if(front)  addFace(bc, id, 4, x, y, z);
				if(back)   addFace(bc, id, 5, x, y, z);
			}
		}
	};

	for(int32 i = 0; i < 8; i++) if(!ndata[i]) goto exit;
	chunk.batching = true;

	// Set write heads to end of center batch
	chunk.genVertsP = chunk.cornerVertsP;
	chunk.genUVsP = chunk.cornerUVsP;
	chunk.genColorsP = chunk.cornerColorsP;
	chunk.batchedVertsLen = chunk.cornerVertsLen;

	for(int x = 0; x < 24; x++) alg(x, 0);
	for(int x = 0; x < 24; x++) alg(x, 23);
	for(int z = 1; z < 23; z++) alg(0, z);
	for(int z = 1; z < 23; z++) alg(23, z);

	chunk.batchedCorners = true;

	exit:
	for(int i = 0; i < 8; i++){
		if(ndata[i]){
			std::lock_guard g(neighbours[i]->stateLock);
			neighbours[i]->beingUsedForBatching = false;
		}
	}
	
	chunk.batching = false;
}


inline void aofc(byte* of, float opaqueness){ // Ambient Occlusion Factor Clamp
	byte val = *of + opaqueness * 54;
	const byte max = 162;
	*of = (val < max)?val:max;
}


void ChunkBatcher::addFace(const BatchContext& bc, byte id, byte face, int8 x, int8 y, int8 z){
	auto& infos = blockDB;
	Chunk& ch = bc.ch;
	Chunk* const* nb = bc.nb;
	const BlockInfo& binfo = infos[id];
	Chunk::ChunkData& data = *bc.ch.blocks;

	float u, v;
	byte of[] = {0, 0, 0, 0}; // Occlusion factor for each vertex on a face.
	const auto& hT = atlas.horizontalTiles;

	switch(face){
	default:
	case 0: { // Top
		u = binfo.topTexId % hT;
		v = binfo.topTexId / hT;

		float a = opaqueness(bc, x,   y+1, z);
		float b = opaqueness(bc, x-1, y+1, z);
		float c = opaqueness(bc, x,   y+1, z-1);
		float d = opaqueness(bc, x,   y+1, z+1);
		float e = opaqueness(bc, x+1, y+1, z);

		aofc(of+0, opaqueness(bc, x-1, y+1, z-1) + a + b + c);
		aofc(of+1, opaqueness(bc, x-1, y+1, z+1) + a + b + d);
		aofc(of+2, opaqueness(bc, x+1, y+1, z+1) + a + d + e);
		aofc(of+3, opaqueness(bc, x+1, y+1, z-1) + a + c + e);
		}
		break;
	case 1: {// Bottom
		u = binfo.bottomTexId % hT;
		v = binfo.bottomTexId / hT;

		float a = opaqueness(bc, x,   y-1, z);
		float b = opaqueness(bc, x-1, y-1, z);
		float c = opaqueness(bc, x,   y-1, z-1);
		float d = opaqueness(bc, x,   y-1, z+1);
		float e = opaqueness(bc, x+1, y-1, z);

		aofc(of+0, opaqueness(bc, x-1, y-1, z-1) + a + b + c);
		aofc(of+1, opaqueness(bc, x+1, y-1, z-1) + a + e + c);
		aofc(of+2, opaqueness(bc, x+1, y-1, z+1) + a + d + e);
		aofc(of+3, opaqueness(bc, x-1, y-1, z+1) + a + d + b);
		}
		break;
	case 2: { // Left
		u = binfo.sidesTexId % hT;
		v = binfo.sidesTexId / hT;
		//npadY = xnpadY;
		float a = opaqueness(bc, x-1, y,   z);
		float b = opaqueness(bc, x-1, y,   z-1);
		float c = opaqueness(bc, x-1, y+1, z);
		float d = opaqueness(bc, x-1, y-1, z);
		float e = opaqueness(bc, x-1, y,   z+1);

		aofc(of+0, opaqueness(bc, x-1, y+1, z-1) + a + b + c);
		aofc(of+1, opaqueness(bc, x-1, y-1, z-1) + a + b + d);
		aofc(of+2, opaqueness(bc, x-1, y-1, z+1) + a + d + e);
		aofc(of+3, opaqueness(bc, x-1, y+1, z+1) + a + c + e);
		}
		break;
	case 3: { // Right
		u = binfo.sidesTexId % hT;
		v = binfo.sidesTexId / hT;
		/*npadY = xnpadY;*/
		float a = opaqueness(bc, x+1, y, z);
		float b = opaqueness(bc, x+1, y+1, z);
		float c = opaqueness(bc, x+1, y, z-1);
		float d = opaqueness(bc, x+1, y, z+1);
		float e = opaqueness(bc, x+1, y-1, z);

		aofc(of+0, opaqueness(bc, x+1, y+1, z-1) + a + b + c);
		aofc(of+1, opaqueness(bc, x+1, y+1, z+1) + a + b + d);
		aofc(of+2, opaqueness(bc, x+1, y-1, z+1) + a + d + e);
		aofc(of+3, opaqueness(bc, x+1, y-1, z-1) + a + c + e);
		}
		break;
	case 4: { // Back
		u = binfo.sidesTexId % hT;
		v = binfo.sidesTexId / hT;
		/*npadY = znpadY;*/

		float a = opaqueness(bc, x, y, z+1);
		float b = opaqueness(bc, x, y-1, z+1);
		float c = opaqueness(bc, x-1, y, z+1);
		float d = opaqueness(bc, x+1, y, z+1);
		float e = opaqueness(bc, x, y+1, z+1);

		aofc(of+0, opaqueness(bc, x-1, y-1, z+1) + a + b + c);
		aofc(of+1, opaqueness(bc, x+1, y-1, z+1) + a + b + d);
		aofc(of+2, opaqueness(bc, x+1, y+1, z+1) + a + d + e);
		aofc(of+3, opaqueness(bc, x-1, y+1, z+1) + a + c + e);
		}
		break;
	case 5: { // Front
		u = binfo.sidesTexId % hT;
		v = binfo.sidesTexId / hT;
		/*npadY = znpadY;*/
		float a = opaqueness(bc, x, y, z-1);
		float b = opaqueness(bc, x, y-1, z-1);
		float c = opaqueness(bc, x-1, y, z-1);
		float d = opaqueness(bc, x+1, y, z-1);
		float e = opaqueness(bc, x, y+1, z-1);

		aofc(of+0, opaqueness(bc, x-1, y-1, z-1) + a + b + c);
		aofc(of+1, opaqueness(bc, x-1, y+1, z-1) + a + e + c);
		aofc(of+2, opaqueness(bc, x+1, y+1, z-1) + a + d + e);
		aofc(of+3, opaqueness(bc, x+1, y-1, z-1) + a + d + b);
		}
		break;
	}
	// Scale of the texture of a single block.
	const auto& tw = atlas.fTileWidth;
	const auto& th = atlas.fTileHeight;

	const uint16 verts = face*4;
	addVertexAlloc(ch, 4);
	for(uint16 i = 0; i < 4; i++){
		uint16 vt = verts + i;
		float vx = Cube::verts_tris[vt*3+0] + x;
		float vy = Cube::verts_tris[vt*3+1] + y;
		float vz = Cube::verts_tris[vt*3+2] + z;

		*ch.genVertsP++ = vx;
		*ch.genVertsP++ = vy;
		*ch.genVertsP++ = vz;

		float _tu = Cube::uvs_tris[vt*2+0];
		float _tv = Cube::uvs_tris[vt*2+1];
		float ux = (_tu + u) * tw;
		float uy = (_tv + v) * th;

		*ch.genUVsP++ = ux;
		*ch.genUVsP++ = uy;

		byte cf_ = 255 - of[i];
		*ch.genColorsP++ = cf_;
		*ch.genColorsP++ = cf_;
		*ch.genColorsP++ = cf_;


		/*const float nx = (Cube::normals_tris[vt*3+0]);
		const float ny = (Cube::normals_tris[vt*3+1] + npadY);
		const float nz = (Cube::normals_tris[vt*3+2]);*/

		/*normalsP++ = nx*127;
		*normalsP++ = ny*127;
		*normalsP++ = nz*127;*/
	}
}

void ChunkBatcher::addVertexAlloc(Chunk& ch, int n){
	ch.batchedVertsLen += n;
	if(ch.batchedVertsLen > ch.allocatedVerts){
		ch.allocatedVerts += 1024;

		float* overts = ch.batchedVerts;
		float* ouvs = ch.batchedUVs;
		byte* ocolors = ch.batchedColors;

		ch.batchedVerts = (float*)realloc(overts, ch.allocatedVerts * 4 * 3);
		ch.batchedUVs = (float*)realloc(ouvs, ch.allocatedVerts * 4 * 2);
		ch.batchedColors = (byte*)realloc(ocolors,  ch.allocatedVerts * 3);

		ch.genVertsP = ch.batchedVerts + (ch.genVertsP - overts);
		ch.genUVsP = ch.batchedUVs + (ch.genUVsP - ouvs);
		ch.genColorsP = ch.batchedColors + (ch.genColorsP - ocolors);

		ch.cornerVertsP = ch.batchedVerts + (ch.cornerVertsP - overts);
		ch.cornerUVsP = ch.batchedUVs + (ch.cornerUVsP - ouvs);
		ch.cornerColorsP = ch.batchedColors + (ch.cornerColorsP - ocolors);
	}
}

float ChunkBatcher::opaqueness(const BatchContext& bc, int8 x, int8 y, int8 z){
	const auto& blocks = *(bc.ch.blocks);
	const auto& nb = bc.nb;
	Block bl;
	if(!nb){	
		if(x >= 0 && x <= 23 && y >= 0 && y <= 23 && z >= 0 && z <= 23){
			bl = blocks[x][y][z];
		} else throw Exception("Illegal!");
	} else {
		// 1 2 3
		// 0   4
		// 7 6 5
		if(x < 0){
			if(z < 0){
				bl = (*nb[7]->blocks)[x + 24][y][z + 24];
			} else if(z > 23){
				bl = (*nb[1]->blocks)[x + 24][y][z - 24];
			} else {
				bl = (*nb[0]->blocks)[x + 24][y][z];
			}
		} else if(x > 23){
			if(z < 0){
				bl = (*nb[5]->blocks)[x - 24][y][z + 24];
			} else if(z > 23){
				bl = (*nb[3]->blocks)[x - 24][y][z - 24];
			} else {
				bl = (*nb[4]->blocks)[x - 24][y][z];
			}
		} else {
			if(z > 23){
				bl = (*nb[2]->blocks)[x][y][z - 24];
			} else if(z < 0){
				bl = (*nb[6]->blocks)[x][y][z + 24];
			} else{
				bl = blocks[x][y][z];
			}
		}	
	}
	return blockDB[bl.id].opaqueness;
}
