#include "SplitPane.h"

SplitPane::SplitPane(){}
SplitPane::~SplitPane(){}

Shared<Component> SplitPane::add(Shared<Component> comp){
	Component::add(comp);
	relayout();
	return comp;
}

void SplitPane::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);
	relayout();
}

void SplitPane::relayout(){
	int size = children.size();
	if(size == 0) return;

	auto& i = insets;

	float x = i.left;
	float y = i.top;
	float w = width - i.right - i.left;
	float h = height - i.bottom - i.top;
	if(orientation == HORIZONTAL){
		children[0]->setBounds(x, y, w * value, h);
		if(size > 1) children[1]->setBounds(x + w * value, y, w * (1 - value), h);
	} else {
		children[0]->setBounds(x, y, w, h * value);
		if(size > 1) children[1]->setBounds(x, y + h * value, w, h * (1 - value));
	}
}

