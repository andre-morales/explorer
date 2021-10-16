#include "TextureAtlas.h"
#include <cmath>
#include <GL/glew.h>

TextureAtlas::TextureAtlas(uint32 tw, uint32 th){
	tileWidth = tw;
	tileHeight = th;
}

TextureAtlas::~TextureAtlas(){}

void TextureAtlas::getTileUV(uint16 x, uint16 y, float* u, float* v){
	*u = tileWidth / (float)width * x;
	*v = tileHeight / (float)height * y;
}

void TextureAtlas::getTileUV(uint16 xy, float* u, float* v){
	uint16 htiles = width / tileWidth;
	uint16 x = xy % htiles;
	uint16 y = xy / htiles;
	getTileUV(x, y, u, v);;
}

void TextureAtlas::getTileSize(uint32* w, uint32* h){
	if(w != nullptr) *w = tileWidth;
	if(h != nullptr) *h = tileHeight;
}

void TextureAtlas::getTileScale(float* w, float* h){
	if(w != nullptr) *w = tileWidth / (float)width;
	if(h != nullptr) *h = tileHeight / (float)height;
}

void TextureAtlas::setMipmap(byte mipmapping){
	Texture::setMipmap(mipmapping, std::log2(width) - std::log2(width/tileWidth));
}
