#include "BorderPane.h"
#include "ilib/types.h"
#include <iostream>

BorderPane::BorderPane(){}
BorderPane::~BorderPane(){}

Shared<Component> BorderPane::add(Shared<Component> comp, int p){
	Component::add(comp, p);
	relayout();
	return comp;
}

void BorderPane::setOrientation(int p, Orientation o){
	positions.insert(positions.begin() + p, o);
}

void BorderPane::setGrowRule(int p, float x, float y){
	growRules.insert(growRules.begin() + p, {x, y});
}

void BorderPane::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	relayout();
}

void BorderPane::relayout(){
	for(uint32 i = 0; i < children.size(); i++){
		auto& child = children[i];

		Orientation ori = Orientation::CENTER_MIDDLE;
		if(i < positions.size()){
			ori = positions[i];
		}

		vec2f gr = {0, 0};
		if(i < growRules.size()){
			gr = growRules[i];
		}

		float ew = width - insets.left - insets.right;
		float eh = height - insets.top - insets.bottom;

		vec4f cb = child->getBounds();
		if(gr.x > 0) cb.width = ew * gr.x;
		if(gr.y > 0) cb.height = eh * gr.y;

		switch(ori){
			case Orientation::LEFT_TOP:
			case Orientation::LEFT_MIDDLE:
			case Orientation::LEFT_BOTTOM:
				cb.x = insets.left;
				break;
			case Orientation::CENTER_TOP:
			case Orientation::CENTER_MIDDLE:
			case Orientation::CENTER_BOTTOM:
				cb.x = insets.left + (ew - cb.width) / 2;
				break;
			case Orientation::RIGHT_TOP:
			case Orientation::RIGHT_MIDDLE:
			case Orientation::RIGHT_BOTTOM:
				cb.x = width - insets.right - cb.width;
				break;
		}
		switch(ori){
			case Orientation::LEFT_TOP:
			case Orientation::CENTER_TOP:
			case Orientation::RIGHT_TOP:
				cb.y = insets.top;
				break;
			case Orientation::LEFT_MIDDLE:
			case Orientation::CENTER_MIDDLE:
			case Orientation::RIGHT_MIDDLE:
				cb.y = insets.top + (eh - cb.height) / 2;
				break;
			case Orientation::LEFT_BOTTOM:
			case Orientation::CENTER_BOTTOM:
			case Orientation::RIGHT_BOTTOM:
				cb.y = height - insets.bottom - cb.height;
				break;
		}

		child->setBounds(cb.x, cb.y, cb.width, cb.height);
	}
}
