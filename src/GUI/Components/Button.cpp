#include "Button.h"
#include "GUI/Text.h"
#include "GUI/Event/MouseButtonEvent.h"
#include "GUI/Event/MouseMotionEvent.h"
#include "Explorer/Window.h"
#include "Render/GLContext.h"
#include "Render/Font.h"
#include <string>
#include <GL/glew.h>
#include <functional>

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

void Button::addActionListener(ActionListener al){
	actionListeners.emplace_back(al);
}

void Button::fireActionListeners(const ActionEvent& ev){
	auto al = actionListeners;
	for(auto& p : al) p(ev);
}
