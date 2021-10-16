#pragma once
#include "Button.h"

class Toggle : public Button {
public:
	using ValueListener = std::function<void(Toggle*, bool)>;
	
	bool checked;

	Toggle();
	Toggle(bool);
	void addValueListener(ValueListener);
	void fireValueListeners();
private:
	std::vector<ValueListener> valueListeners;
	
	Toggle(const Toggle&) = delete;
};

