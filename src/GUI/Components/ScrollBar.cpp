#include "ScrollBar.h"
#include "GUI/Components/Button.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Event/MouseButtonEvent.h"
#include "GUI/Event/MouseMotionEvent.h"

ScrollBar::ScrollBar() : ScrollBar(0){}
ScrollBar::ScrollBar(float v) : value(v){
	setButton(Sh<Button>());
	setLayout(new StackLayout());
	setValue(v);
}

void ScrollBar::setValue (float v) {
	value = v;
	button->x = (v - min)/(max - min) * (width - button->width);
	fireValueListeners();
}

void ScrollBar::setButton(sh<Button> btn){
	if(button){
		remove(button);
	}
	add(button = btn);
	btn->addMouseButtonListener([this](const MouseButtonEvent& me){
		if(me.isPress()){
			held = true;
			clickX = me.sx;
			btnBeginX = button->x;
		} else if(me.isRelease()) held = false;
	});
	addMouseMotionListener([this](const MouseMotionEvent& me){
		if(held){
			button->x = btnBeginX + (me.sx - clickX);
			if(button->x < 0) button->x = 0;
			else if(button->x + button->width > width){
				button->x = width - button->width;
			}
			float norm = button->x / (width - button->width); // Normal value from 0.0 to 1.0
			float rang = min + (max - min) * norm;            // Ranged value from Min to Max
			rang += increment/2.0;                            // Pad half an increment so that fmod works.
			float fval = rang - std::fmod(rang, increment);   // Remove remainder and get final value.
			if(value != fval){
				value = fval;
				fireValueListeners();
			}
		} 
	}, true);
}
void ScrollBar::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	float bw = w/5;
	float bx = (value - min)/(max - min) * (width - bw);
	button->setBounds(bx, 0, bw, h);
}
void ScrollBar::addValueListener(ValueListener vl){
	valueListeners.emplace_back(vl);
}
void ScrollBar::fireValueListeners(){
	for(auto& vl : valueListeners){
		vl(value);
	}
}