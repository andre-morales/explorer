#include "Texture.h"

class TextureAtlas : public Texture {
public:
	uint32 tileWidth, tileHeight;

	TextureAtlas(uint32, uint32);
	~TextureAtlas();
	void getTileUV(uint16, float*, float*);
	void getTileUV(uint16, uint16, float*, float*);
	void getTileSize(float*, float*);
	void setMipmap(byte mipmapping) override;
};