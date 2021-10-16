#include "SplitLayout.h"
#include "GUI/Components/Component.h"

SplitLayout::SplitLayout(Orientation o, float v) : orientation(o), value(v){}

void SplitLayout::added(Component&){
	relayout();
}

void SplitLayout::imp_relayout(Component& comp) {
	int size = comp.children.size();
	if(size == 0) return;

	auto& i = comp.insets;

	float x = i.left;
	float y = i.top;
	float w = comp.width - i.right - i.left;
	float h = comp.height - i.bottom - i.top;
	if(orientation == HORIZONTAL){
		comp.children[0]->setBounds(x, y, w * value, h);
		if(size > 1) comp.children[1]->setBounds(x + w * value, y, w * (1 - value), h);
	} else {
		comp.children[0]->setBounds(x, y, w, h * value);
		if(size > 1) comp.children[1]->setBounds(x, y + h * value, w, h * (1 - value));
	}
}