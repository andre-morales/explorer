#pragma once
#include "ilib/Math/vec4.h"
#include "ilib/mem.h"
#include <string>

class GLContext;
class Font;
struct Color;

class Text {
public:
	enum class Orientation {
		LEFT_TOP,    CENTER_TOP,    RIGHT_TOP,
		LEFT_MIDDLE, CENTER_MIDDLE, RIGHT_MIDDLE,
		LEFT_BOTTOM, CENTER_BOTTOM, RIGHT_BOTTOM,
	};

	Orientation textOrientation = Orientation::CENTER_MIDDLE;
	Shared<Font> textFont;

    Text();
    ~Text();

    void setFont(Shared<Font>);
    void draw(GLContext&, std::string, vec4f, Color);
};
