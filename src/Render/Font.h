#pragma once
#include <math/vec4.h>
#include <string>
#include "ilib/mem.h"
class TextureAtlas;
class GLContext;
class Color;
class Font {
public:
	Shared<TextureAtlas> fontAtlas;
	float chWidth, chHeight;
	float aspect;

	Font(Shared<TextureAtlas>);
	void drawString(GLContext& glc, const std::string&, const Color&);
	void drawShadowString(GLContext& glc, const std::string&, const Color&, const Color&, float, float);
};
