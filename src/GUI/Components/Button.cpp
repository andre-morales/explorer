#include "Button.h"
#include "GUI/Event/MouseButtonEvent.h"
#include "GUI/Event/MouseMotionEvent.h"
#include "Explorer/Window.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include <string>
#include <GL/glew.h>

Button::Button() : Button("Button"){}
Button::Button(const std::string& text){
	this->text = text;
	focusable = true;
	mouseHeldOver = false;
	addMouseButtonListener([this](const MouseButtonEvent& ev){
		if(ev.isLeftButton()){
			if(ev.isPress()){
				mouseHeldOver = true;
			} else if(mouseHeldOver){
				fireActionListeners(ActionEvent(this));
			}
		}
	});
	addMouseMotionListener([this](const MouseMotionEvent& ev){
		if(ev.isLeaving()){
			mouseHeldOver = false;
		}
	});
}

void Button::paintComponent(GLContext& gi, vec2f anch){
	Component::paintComponent(gi, anch);
	if(font){
		gi.enableTexture2d();
		gi.enableAlphaTesting();
		glPushMatrix();
		glTranslatef(anch.x, anch.y, 0);
		const int chars = text.length();
		const float aw = width;
		const float ah = height;
		const float w = aw -insets.le - insets.ri;
		const float h = ah -insets.to - insets.bo;

		const float sw = 1.0/gi.getAspect() * font->aspect; // Scale Width, combination of aspect ratios.
		const float hs = h * sw;
		const float cr = w/chars; // Char Ratio, Width by char
		if(hs > cr){
			glTranslatef(insets.le, (ah - cr/sw)/2, 0);
			glScalef(cr, cr/sw, 1);
		} else {
			glTranslatef((w - hs*chars)/2, insets.to, 0);
			glScalef(hs, h, 1);
		}
		font->drawString(gi, text.c_str(), {1, 1, 1, 1});
		glPopMatrix();
	}
}

void Button::addActionListener(ActionListener al){
	actionListeners.emplace_back(al);
}

void Button::fireActionListeners(const ActionEvent& ev){
	auto al = actionListeners;
	for(auto& p : al) p(ev);
}
