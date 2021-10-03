#pragma once
#include "Game/Chunk.h"
#include "ilib/types.h"
class Instance;
class Planet;
class BlockInfo;

class ChunkBatcher {
public:
	ChunkBatcher(Planet&);
	void batch(Chunk&);
	void batchCenter(Chunk&);
	void batchCorners(Chunk&);

private:
	const Planet& planet;
	const Instance& dataInstance;
	const std::vector<BlockInfo>& blockDB;

	struct {
		uint32 tileWidth, tileHeight;
		uint32 width, height;
		uint8 horizontalTiles;
		float fTileWidth, fTileHeight;
	} atlas;

	struct BatchContext{
		Chunk& ch;
		Chunk* const* nb;
	};

	void addFace(const BatchContext&, byte, byte, int8, int8, int8);
	float opaqueness(const BatchContext&, int8, int8, int8);
	static void addVertexAlloc(Chunk&, int);
};