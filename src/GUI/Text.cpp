#include "Text.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include "Render/Color.h"
#include <GL/glew.h>
#include <iostream>
#include <string>

Text::Text(){}
Text::~Text(){}

void Text::setFont(Shared<Font> font){
	this->textFont = font;
}

void Text::draw(GLContext& gl, const std::string& str, const vec4f& rect, const Color& color){
	Font& font = *textFont;
	gl.enableUVsArray();
	gl.enableTexture2d();
	gl.enableAlphaTesting();
	glPushMatrix();
	glTranslatef(rect.x, rect.y, 0);

	int llc = 0; // Chars of longest line
	int lines = 0;
	{
		int strLen = str.length();
		int count = 0;
		for(int i = 0; i < strLen; i++){
			char c = str[i];
			if(c == '\n'){
				if(count > llc) llc = count;
				count = 0;
			} else if(c != '\r'){
				if(count == 0) lines++;
				count++;
			}
		}
		if(count > llc) llc = count;
	}

	float w = rect.width;
	float h = rect.height;
	float th = font.charHeight * lines;
	float tw = font.charWidth * llc;

	float scale;
	if(textSize < 0){
		float scaleX = rect.height / th;
		float scaleY = rect.width / tw;
		scale = (scaleX > scaleY)?scaleY:scaleX;
	} else {
		scale = textSize;
	}

	switch(textOrientation){
	    default:
		case Orientation::LEFT_TOP:
			glTranslatef(0, 0, 0);
			break;
		case Orientation::LEFT_BOTTOM:
			glTranslatef(0, (h - th * scale), 0);
			break;
		case Orientation::CENTER_MIDDLE:
			glTranslatef((w - tw * scale) / 2, (h - th * scale) / 2, 0);
			break;

	}

	glScalef(font.charWidth * scale, font.charHeight * scale, 1);

	glColor4f(color.r, color.g, color.b, color.a);
	font.drawString(gl, str, color);
	glPopMatrix();
};

float Text::getBodyHeight(const std::string& str){
	int lines = 0;
	int strLen = str.length();
	for(int i = 0; i < strLen; i++){
		if(str[i] == '\n'){
			lines++;
		}
	}

	return textFont->charHeight * lines * textSize;
}
