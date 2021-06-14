#pragma once
#include "ilib/Math/vec4.h"
#include "ilib/mem.h"
#include "ilib/types.h"
#include <string>

class TextureAtlas;
class GLContext;
class Color;

class Font {
public:
	Shared<TextureAtlas> fontAtlas;
	uint32 charWidth = 0, charHeight = 0;
	float charSW = 0, charSH = 0;
	float aspect = 0;

    Font();
	Font(Shared<TextureAtlas>);
	void setTexture(Shared<TextureAtlas>);
    void drawString(GLContext&, const std::string&, const Color&);
	void drawShadowString(GLContext&, const std::string&, const Color&, const Color&, float, float);
private:
    Font(const Font&) = delete;
};
