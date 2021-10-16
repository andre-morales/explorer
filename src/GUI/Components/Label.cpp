#include "Label.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include <GL/glew.h>

Label::Label() : Label(""){}

Label::Label(const std::string& text) : Component(){
	this->text = text;
    this->background = false;
    this->enabled = false;
}

void Label::paintComponent(GLContext& gi, vec2f anch){
	Component::paintComponent(gi, anch);

    if(textFont){
        vec4f rect(
            anch.x + insets.left,
            anch.y + insets.top,
            width - insets.left - insets.right,
            height - insets.top - insets.bottom
	    );

        Text::draw(gi, text, rect, fgColor);
	}
}
