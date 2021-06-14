#include "Text.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include "Render/Color.h"
#include <GL/glew.h>
#include <iostream>

Text::Text(){}
Text::~Text(){}

void Text::setFont(Shared<Font> font){
    this->textFont = font;
}

void Text::draw(GLContext& gl, std::string str, vec4f rect, Color color){
    Font& font = *textFont;

	gl.enableTexture2d();
	gl.enableAlphaTesting();

	glPushMatrix();
	glTranslatef(rect.x, rect.y, 0);
	int chars = str.length();
	float w = rect.width;
	float h = rect.height;
    float th = font.charHeight;
    float tw = font.charWidth * chars;

	float scaleX = rect.height / th;
	float scaleY = rect.width / tw;
	float scale = std::min(scaleX, scaleY);

	glTranslatef((w - tw * scale) / 2, (h - th * scale) / 2, 0);
	glScalef(font.charWidth * scale, font.charHeight * scale, 1);

	//glColor4f(color.r, color.g, color.b, color.a);
	font.drawString(gl, str.c_str(), color);
	glPopMatrix();
};
