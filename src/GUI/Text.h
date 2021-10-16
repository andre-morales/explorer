#pragma once
#include "GUI/Orientation.h"
#include "ilib/Math/vec4.h"
#include "ilib/mem.h"
#include <string>

class GLContext;
class Font;
class Color;

class Text {
public:
	Orientation textOrientation = Orientation::CENTER_MIDDLE;
	Shared<Font> textFont;
	float textSize = -1;

	Text() = default;
	virtual ~Text() = default;

    void setFont(Shared<Font>);
    void draw(GLContext&, const std::string&, const vec4f&, const Color&);
    float getBodyHeight(const std::string&);
};
