#include "TextField.h"
#include "Label.h"
#include "GUI/Event/KeyEvent.h"
#include <iostream>
#include <memory>

using namespace std;

TextField::TextField() : TextField(""){}
TextField::TextField(const string& txt) : Component(){
	focusable = true;
	this->text = txt;
	lblText = mkShared<Label>(text);
	lblText->setBackgroundColor({1.0, 0.0, 0.0, 0.0});
	/*lblText->insets = {0.025, 0.075, 0.025, 0.075};*/

	addKeyListener([this](const KeyEvent& ev){
		if(ev.isPress()){
			if(ev.isBackspace()){
				if(text.length() > 0){
					text.pop_back();
					lblText->text = text;
				}
			} else {
				char c = ev.getChar();
				if(c != 0){
					text += c;
					lblText->text = text;
				}
			}
		}
	});
}

void TextField::paintComponent(GLContext& gi, vec2f anch) {
	Component::paintComponent(gi, anch);
	lblText->paint(gi, anch);
}

void TextField::setPlaceHolder(const string& ph){
	placeHolder = ph;
}

void TextField::setFont(Shared<Font> font) {
	Component::setFont(font);
	lblText->setFont(font);
}

void TextField::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	lblText->setBounds(x, y, w, h);

};