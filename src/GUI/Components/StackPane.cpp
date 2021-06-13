#include "StackPane.h"

StackPane::StackPane(){}
StackPane::~StackPane(){}

Shared<Component> StackPane::add(Shared<Component> comp){
	Component::add(comp);

	auto& i = insets;
	comp->setBounds(i.le, i.to, width - i.le - i.ri, height - i.to - i.bo);
	return comp;
}

void StackPane::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);

	for(auto& c : children){
		auto& i = insets;
		c->setBounds(i.le, i.to, w - i.le - i.ri, h - i.to - i.bo);
	}
}
