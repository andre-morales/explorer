#pragma once
struct ActionEvent {
	Component* const source;
	ActionEvent(Component* src) : source(src){}
};
