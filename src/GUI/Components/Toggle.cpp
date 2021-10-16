#include "Toggle.h"

Toggle::Toggle() : Toggle(false){}

Toggle::Toggle(bool c) : checked(c){
	addActionListener([this](auto&){
		checked = !checked;

		fireValueListeners();
	});
}


void Toggle::addValueListener(ValueListener vl){
	valueListeners.emplace_back(vl);
}

void Toggle::fireValueListeners(){
	for(auto& vl : valueListeners) {
		vl(this, checked);
	}
}