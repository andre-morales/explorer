#include "Texture.h"
#include "Image.h"
#include "GL/glew.h"
#include <cstring>
#include <cmath>
#include <algorithm>
#include <initializer_list>

Texture::Texture() : Texture(nullptr, false, false){}
Texture::Texture(const char* path) : Texture(Image(path)){}
Texture::Texture(bool lmin, bool lmag) : Texture(nullptr, lmin, lmag){}
Texture::Texture(const Image& img) : Texture(&img, false, false){}
Texture::Texture(const Image& img, bool lmin, bool lmag) : Texture(&img, lmin, lmag){}
Texture::Texture(const Image* img, bool lmin, bool lmag){
	this->glId = 0;
	this->linearMin = lmin;
	this->linearMag = lmag;
	this->mipmapping = 0;
	this->glConfigChanged = true;
	set(img);
}
Texture::~Texture(){
	if(glId != 0) glDeleteTextures(1, &glId);
}

void Texture::set(const Image* image){
	if(image){
		set(*image);
	} else {
		if(glId != 0) glDeleteTextures(1, &glId);
		this->glId = 0;
	}
}

void Texture::set(const Image& image){
	width = image.width;
	height = image.height;

	baseTexture.reset(new uint8[width * height * 4]);
	memcpy(baseTexture.get(), image.data.get(), (uint64)width * height * 4);

	glTexChanged = true;
}

void Texture::setLinearMagn(bool linear){
	linearMag = linear;
	glConfigChanged = true;
}
void Texture::setLinearMini(bool linear){
	linearMin = linear;
	glConfigChanged = true;
}
void Texture::bind(){
	if(glId == 0){
		glGenTextures(1, &glId);
		glBindTexture(GL_TEXTURE_2D, glId);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		glBindTexture(GL_TEXTURE_2D, glId);
	}
	if(glConfigChanged){
		glConfigChanged = false;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linearMin)?GL_LINEAR:GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linearMag)?GL_LINEAR:GL_NEAREST);
	}
	if(glTexChanged){
		glTexChanged = false;
		if(baseTexture){
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, baseTexture.get());
		}
	}
}

void Texture::setMipmap(byte mipmapping){
	setMipmap(mipmapping, std::log2(std::min(width, height)));
}
void Texture::setMipmap(byte mipmapping, byte levels){
	this->mipmapping = mipmapping;
	glBindTexture(GL_TEXTURE_2D, glId);
	if(mipmapping) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linearMin)?GL_LINEAR_MIPMAP_LINEAR:GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linearMag)?GL_LINEAR:GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 2);
		switch(mipmapping){
			case 1:
				mipmap(levels);
				break;
			case 2:
				glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, baseTexture.get());
				break;
		}
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (linearMin)?GL_LINEAR:GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (linearMag)?GL_LINEAR:GL_NEAREST);
	}
}

void Texture::mipmap(byte levels){
	glBindTexture(GL_TEXTURE_2D, glId);
	uint8* img = baseTexture.get();
	uint8* limg = img;
	uint8* mimg;
	uint32 mw = width;
	uint32 mh = height;
	for(int i = 1; mw > 1 && mh > 1 && i <= levels; i++){
		mw /= 2;
		mh /= 2;

		mimg = new uint8[mw * mh * 4];

		uint8* dst = mimg;
		for(uint32 y = 0; y < mh; y++){
			uint32 csrc = (y * 2 * mw * 2) * 4;
			for(uint32 x = 0; x < mw; x++){
				uint32 _csrc = (y * 2 * mw * 2 + x * 2) * 4;
				uint32 rsrc = _csrc + 4;
				uint32 bsrc = _csrc + mw * 2 * 4;
				*dst++ = (limg[csrc++] + limg[rsrc++] + limg[bsrc++]) / 3;
				*dst++ = (limg[csrc++] + limg[rsrc++] + limg[bsrc++]) / 3;
				*dst++ = (limg[csrc++] + limg[rsrc++] + limg[bsrc++]) / 3;
				*dst++ = (limg[csrc++] + limg[rsrc++]*0 + limg[bsrc++]*0) / 1;
				csrc+=4;
			}
		}
		if(limg != img) delete[] limg;
		limg = mimg;
		glTexImage2D(GL_TEXTURE_2D, i, GL_RGBA, mw, mh, 0, GL_RGBA, GL_UNSIGNED_BYTE, mimg);
	}
}
