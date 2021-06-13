#include "Label.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include <GL/glew.h>

Label::Label() : Label(""){}

Label::Label(const std::string& text) : Component(){
	this->text = text;
}

void Label::paintComponent(GLContext& gi, vec2f anch){
	Component::paintComponent(gi, anch);
	if(!font) return;

	gi.enableTexture2d();
	gi.enableAlphaTesting();
	glPushMatrix();
	glTranslatef(anch.x, anch.y, 0);
	const int chars = text.length();
	const float aw = width;
	const float ah = height;
	const float w = aw * (1-insets.le-insets.ri);
	const float h = ah * (1-insets.to-insets.bo);

	const float sw = 1.0/gi.getAspect() * font->aspect; // Scale Width, combination of aspect ratios.
	const float hs = h * sw;
	const float cr = w/chars; // Char Ratio, Width by char
	if(hs > cr){
		glTranslatef(insets.le*aw, (ah - cr/sw)/2, 0);
		glScalef(cr, cr/sw, 1);
	} else {
		glTranslatef((w - hs*chars)/2, insets.to*ah, 0);
		glScalef(hs, h, 1);
	}
	font->drawString(gi, text, fgColor);
	glPopMatrix();
}
