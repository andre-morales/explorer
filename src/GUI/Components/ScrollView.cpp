#include "ScrollView.h"

ScrollView::ScrollView(){}
ScrollView::~ScrollView(){}

Shared<Component> ScrollView::add(Shared<Component> comp, int p){
	Component::add(comp, p);

	auto& i = insets;
	comp->setBounds(i.le, i.to, width - i.le - i.ri, height - i.to - i.bo);
	return comp;
}

void ScrollView::setBounds(float x, float y, float w, float h){
	Component::setBounds(x, y, w, h);

	for(auto& c : children){
		auto& i = insets;
		c->setBounds(i.le, i.to, w - i.le - i.ri, h - i.to - i.bo);
	}
}

void ScrollView::paintChildren(GLContext& c, vec2f anch){
	for (auto& child : children) {
		child->paint(c, {anch.x + child->x + scrollX, anch.y + child->y + scrollY});
	}
}
