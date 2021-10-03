#pragma once
class ActionEvent {
public:
	Component* const source;
	ActionEvent(Component* src) : source(src){}
};
