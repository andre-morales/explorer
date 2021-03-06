#include "TextField.h"
#include "GUI/Components/Label.h"
#include "GUI/Event/CharEvent.h"
#include "GUI/Event/KeyEvent.h"

TextField::TextField() : TextField(""){}
TextField::TextField(const std::string& txt){
	focusable = true;
	this->text = txt;

	addKeyListener([this](const KeyEvent& ev){
		if((ev.isPress() || ev.isRepeat()) && ev.isBackspace()){
			if(text.length() > 0){
				text.pop_back();
			}
		}
	});
	addCharListener([this](const CharEvent& ev){
		char c = ev.getChar();
		if(c != 0){
			text += c;
		}
	});
}

void TextField::paintComponent(GLContext& gi, vec2f anch) {
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

void TextField::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
};
