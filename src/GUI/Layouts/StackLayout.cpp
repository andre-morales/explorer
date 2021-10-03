#include "StackLayout.h"
#include "GUI/Components/Component.h"
StackLayout::StackLayout(){}
StackLayout::~StackLayout(){}

void StackLayout::imp_relayout(Component& comp) {
	auto& i = component->insets;
	for(auto& c : component->children){
		c->setBounds(i.le, i.to, component->width - i.le - i.ri, component->height - i.to - i.bo);
	}
}
void StackLayout::added(Component& c){
	auto& i = component->insets;
	c.setBounds(i.le, i.to, component->width - i.le - i.ri, component->height - i.to - i.bo);
}