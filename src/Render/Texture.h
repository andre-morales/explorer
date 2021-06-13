#pragma once
#include <vector>
#include <ilib/types.h>
#include <memory>
class Image;

class Texture {
public:
	uint32 glId, width, height;
	std::unique_ptr<uint8[]> baseTexture;
	byte mipmapping;
	bool linearMin, linearMag;

	Texture();
	Texture(bool, bool);
	Texture(const char*);
	Texture(const Image&);
	Texture(const Image&, bool, bool);
	Texture(const Image*, bool, bool);

	Texture(const Texture&) = delete;
	virtual ~Texture();

	void set(const Image*);
	void set(const Image&);
	void setLinearMagn(bool);
	void setLinearMini(bool);
	void bind();

	virtual void setMipmap(byte);
	virtual void setMipmap(byte, byte);
	virtual void mipmap(byte);
private:
	bool glTexChanged, glConfigChanged;
};
