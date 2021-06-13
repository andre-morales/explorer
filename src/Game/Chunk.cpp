#include "Chunk.h"
#include "Explorer/Explorer.h"
#include "Render/Renderer.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Block.h"
#include "Game/BlockInfo.h"
#include "Math/vec3.h"
#include <vector>

#ifndef EXPLORER_SERVER
    #include "Render/Geometry.h"
    #include "Render/TextureAtlas.h"
#endif

Chunk::Chunk(Planet* pl, int32 x, int32 y, int32 z) : planet(pl){
    blocks = nullptr;
    cx = x;
    cy = y;
    cz = z;
    id = ((cy * 16384) + cz) * 16384 + cx;

    allocatedVerts = 0;
    batchedVertsLen = 0;
    batchedVerts = nullptr;
    genVertsP = nullptr;
    batchedUVs = nullptr;
    genUVsP = nullptr;
}
Chunk::~Chunk(){
}

void Chunk::gen(){
    blocks = (ChunkData*)malloc(24 * 24 * 24 * sizeof(Block));

    (*blocks)[12][12][12] = Block{1};
    /*for(int x = 0; x < 24; x++){
     for(int y = 0; y < 24; y++){
        for(int z = 0; z < 24; z++){
        (*blocks)[x][y][z] = Block{2};
    }
    }
    }*/

}

void Chunk::batch(){
    const std::vector<BlockInfo>& infos = planet->universe->instance->registeredBlocks;
    ChunkData& data = *blocks;

    genVertsP = batchedVerts;
	genUVsP = batchedUVs;
	batchedVertsLen = 0;

	for(int x = 0; x < 24; x++){
		for(int y = 0; y < 24; y++){
			for(int z = 0; z < 24; z++){
				const vec3i bpos(x, y, z);

				Block& block = data[x][y][z];
				uint8 id = block.id;

				if(id!=0){
					byte topBlock = (y < 23)?data[x][y + 1][z].id:1;
					byte bottomBlock = (y > 0)?data[x][y - 1][z].id:1;
					byte leftBlock = 0;
					byte rightBlock = 0;
					byte frontBlock = 0;
					byte backBlock = 0;

					const auto& topInfo = infos[topBlock];
					const auto& bottomInfo = infos[bottomBlock];

					/*if(x > 0) leftBlock = data[x-1][y][z].id;
					else if(lS) leftBlock = lS->data[23][y][z].id;

					if(x < 23) rightBlock = data[x+1][y][z].id;
					else if(rS) rightBlock = rS->data[0][y][z].id;

					if(z < 23)frontBlock = data[x][y][z + 1].id;
					else if(fS)frontBlock = fS->data[x][y][0].id;

					if(z > 0) backBlock = data[x][y][z - 1].id;
					else if(bS) backBlock = bS->data[x][y][23].id;*/

                    bool top = !topInfo.opaque;
                    bool bottom = !bottomInfo.opaque;
                    bool left = !infos[leftBlock].opaque;
                    bool right = !infos[rightBlock].opaque;
                    bool front = !infos[frontBlock].opaque;
                    bool back = !infos[backBlock].opaque;
                    if(top) addFace(id, 0, x, y, z);
                    if(bottom) addFace(id, 1, x, y, z);
                    if(left) addFace(id, 2, x, y, z);
                    if(right) addFace(id, 3, x, y, z);
                    if(front) addFace(id, 4, x, y, z);
                    if(back) addFace(id, 5, x, y, z);
				}
			}
		}
	}
}

inline void aofc(byte* of, float opaqueness){ // Ambient Occlusion Factor Clamp
	byte val = *of + opaqueness * 54;
	const byte max = 162;
	*of = (val < max)?val:max;
}

void Chunk::addFace(byte id, byte face, byte x, byte y, byte z){
    #ifndef EXPLORER_SERVER
    const Instance& inst = *planet->universe->instance;

	const std::vector<BlockInfo>& infos = inst.registeredBlocks;
	const BlockInfo& binfo = infos[id];
	const TextureAtlas& atlas = *inst.explorer->renderer->blockAtlas;

 	// Scale of the texture of a single block.
	const float uvScaleX = atlas.tileWidth / (float)atlas.width;
	const float uvScaleY = atlas.tileHeight / (float)atlas.height;
	float u, v;
	/*byte of[] = {0, 0, 0, 0}; // Occlusion factor for each vertex on a face.
	float npadY = 0;
	float xnpadY = 0.20;
	float znpadY = 0.40;*/
	uint16 atWTiles = atlas.width / atlas.tileWidth;

	switch(face){
	default:
	case 0: { // Top
		u = binfo.topTexId % atWTiles;
		v = binfo.topTexId / atWTiles;
/*
		float a = sOpaqueness(x,   y+1, z);
		float b = sOpaqueness(x-1, y+1, z);
		float c = sOpaqueness(x,   y+1, z-1);
		float d = sOpaqueness(x,   y+1, z+1);
		float e = sOpaqueness(x+1, y+1, z);

		aofc(of+0, sOpaqueness(x-1, y+1, z-1) + a + b + c);
		aofc(of+1, sOpaqueness(x-1, y+1, z+1) + a + b + d);
		aofc(of+2, sOpaqueness(x+1, y+1, z+1) + a + d + e);
		aofc(of+3, sOpaqueness(x+1, y+1, z-1) + a + c + e);*/
		}
		break;
	case 1: {// Bottom
		u = binfo.bottomTexId % atWTiles;
		v = binfo.bottomTexId / atWTiles;
/*
		float a = sOpaqueness(x,   y-1, z);
		float b = sOpaqueness(x-1, y-1, z);
		float c = sOpaqueness(x,   y-1, z-1);
		float d = sOpaqueness(x,   y-1, z+1);
		float e = sOpaqueness(x+1, y-1, z);

		aofc(of+0, sOpaqueness(x-1, y-1, z-1) + a + b + c);
		aofc(of+1, sOpaqueness(x+1, y-1, z-1) + a + e + c);
		aofc(of+2, sOpaqueness(x+1, y-1, z+1) + a + d + e);
		aofc(of+3, sOpaqueness(x-1, y-1, z+1) + a + d + b);*/
		}
		break;
	case 2: { // Left
		u = binfo.sidesTexId % atWTiles;
		v = binfo.sidesTexId / atWTiles;
		/*npadY = xnpadY;
		float a = sOpaqueness(x-1, y,   z);
		float b = sOpaqueness(x-1, y,   z-1);
		float c = sOpaqueness(x-1, y+1, z);
		float d = sOpaqueness(x-1, y-1, z);
		float e = sOpaqueness(x-1, y,   z+1);

		aofc(of+0, sOpaqueness(x-1, y+1, z-1) + a + b + c);
		aofc(of+1, sOpaqueness(x-1, y-1, z-1) + a + b + d);
		aofc(of+2, sOpaqueness(x-1, y-1, z+1) + a + d + e);
		aofc(of+3, sOpaqueness(x-1, y+1, z+1) + a + c + e);*/
		}
		break;
	case 3: { // Right
		u = binfo.sidesTexId % atWTiles;
		v = binfo.sidesTexId / atWTiles;
		/*npadY = xnpadY;
		float a = sOpaqueness(x+1, y, z);
		float b = sOpaqueness(x+1, y+1, z);
		float c = sOpaqueness(x+1, y, z-1);
		float d = sOpaqueness(x+1, y, z+1);
		float e = sOpaqueness(x+1, y-1, z);

		aofc(of+0, sOpaqueness(x+1, y+1, z-1) + a + b + c);
		aofc(of+1, sOpaqueness(x+1, y+1, z+1) + a + b + d);
		aofc(of+2, sOpaqueness(x+1, y-1, z+1) + a + d + e);
		aofc(of+3, sOpaqueness(x+1, y-1, z-1) + a + c + e);*/
		}
		break;
	case 4: { // Back
		u = binfo.sidesTexId % atWTiles;
		v = binfo.sidesTexId / atWTiles;
		/*npadY = znpadY;

		float a = sOpaqueness(x, y, z+1);
		float b = sOpaqueness(x, y-1, z+1);
		float c = sOpaqueness(x-1, y, z+1);
		float d = sOpaqueness(x+1, y, z+1);
		float e = sOpaqueness(x, y+1, z+1);

		aofc(of+0, sOpaqueness(x-1, y-1, z+1) + a + b + c);
		aofc(of+1, sOpaqueness(x+1, y-1, z+1) + a + b + d);
		aofc(of+2, sOpaqueness(x+1, y+1, z+1) + a + d + e);
		aofc(of+3, sOpaqueness(x-1, y+1, z+1) + a + c + e);*/
		}
		break;
	case 5: { // Front
		u = binfo.sidesTexId % atWTiles;
		v = binfo.sidesTexId / atWTiles;
		/*npadY = znpadY;
		float a = sOpaqueness(x, y, z-1);
		float b = sOpaqueness(x, y-1, z-1);
		float c = sOpaqueness(x-1, y, z-1);
		float d = sOpaqueness(x+1, y, z-1);
		float e = sOpaqueness(x, y+1, z-1);

		aofc(of+0, sOpaqueness(x-1, y-1, z-1) + a + b + c);
		aofc(of+1, sOpaqueness(x-1, y+1, z-1) + a + e + c);
		aofc(of+2, sOpaqueness(x+1, y+1, z-1) + a + d + e);
		aofc(of+3, sOpaqueness(x+1, y-1, z-1) + a + d + b);*/
		}
		break;
	}
	float pixelCorrection = 0.005; // Use 2 for full pixel, 1 for half pixel.
	float hfx = 1.0 / atlas.width;
	float hfy = 1.0 / atlas.height;
	const uint16 verts = face*4;
	addVertexAlloc(4);
	for(uint16 i = 0; i < 4; i++){
		 // Creates two triangles out of a quad.
		int index;
		//if(i < 3){
			index = i;
		//} else {
		//	index = (i - 1) % 4;
		//}
		uint16 vt = verts + index;
		const float vx = Cube::verts_tris[vt*3+0] + x;
		const float vy = Cube::verts_tris[vt*3+1] + y;
		const float vz = Cube::verts_tris[vt*3+2] + z;

		/*const float nx = (Cube::normals_tris[vt*3+0]);
		const float ny = (Cube::normals_tris[vt*3+1] + npadY);
		const float nz = (Cube::normals_tris[vt*3+2]);*/

		const float _tu = Cube::uvs_tris[vt*2+0];
		const float _tv = Cube::uvs_tris[vt*2+1];
		float ux = (_tu + u) * uvScaleX;
		float uy = (_tv + v) * uvScaleY;

		ux -= hfx * (_tu - 0.5) * pixelCorrection;
		uy -= hfy * (_tv - 0.5) * pixelCorrection;

		*genVertsP++ = vx;
		*genVertsP++ = vy;
		*genVertsP++ = vz;

		/*normalsP++ = nx*127;
		*normalsP++ = ny*127;
		*normalsP++ = nz*127;*/

		*genUVsP++ = ux;
		*genUVsP++ = uy;

		/*byte cf_ = 255 - of[index];
		*colorsP++ = cf_;
		*colorsP++ = cf_;
		*colorsP++ = cf_;*/
	}
	#endif // EXPLORER_SERVER
}

void Chunk::addVertexAlloc(){
	addVertexAlloc(1);
}

void Chunk::addVertexAlloc(int n){
	batchedVertsLen += n;
	if(batchedVertsLen > allocatedVerts){
		allocatedVerts += 1024;

		float* overts = this->batchedVerts;
		float* ouvs = this->batchedUVs;
		/*int8* onormals = this->normals;
		byte* ocolors = this->colors;*/

		this->batchedVerts = (float*)realloc(overts, allocatedVerts * 4 * 3);
		this->batchedUVs = (float*)realloc(ouvs, allocatedVerts * 4 * 2);
		/*this->normals = (int8*)realloc(onormals, vertsAlloc * 3);
		this->colors = (byte*)realloc(ocolors, vertsAlloc * 3);*/

		this->genVertsP = this->batchedVerts + (genVertsP - overts);
		this->genUVsP = this->batchedUVs + (genUVsP - ouvs);
		/*this->normalsP = this->normals + (normalsP - onormals);
		this->colorsP = this->colors + (colorsP - ocolors);*/
	}
}

bool Chunk::sNotAir(int32 x, int32 y, int32 z){
	/*return world.getGenBlockAt(sx*24+x, sy*24+y, sz*24+z) != 0;*/
	return false;
}

bool Chunk::sOpaque(int32 x, int32 y, int32 z){
	/*return world.explorer.blocks[world.getGenBlockAt(sx*24+x, sy*24+y, sz*24+z)].opaque;*/
	return false;
}

float Chunk::sOpaqueness(int32 x, int32 y, int32 z){
    return false;
	/*return world.explorer.blocks[world.getGenBlockAt(sx*24+x, sy*24+y, sz*24+z)].opaqueness;*/
}

