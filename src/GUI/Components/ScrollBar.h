#pragma once
#include "GUI/Components/Component.h"

class ScrollBar : public Component {
public:
	using ValueListener = std::function<void(float)>;
	std::vector<ValueListener> valueListeners;

	enum Orientation{
		VERTICAL, HORIZONTAL
	} orientation = Orientation::HORIZONTAL;

	sh<class Button> button;
	float min = 0;
	float max = 1;
	float increment = 0.01;
	float value = 0;

	ScrollBar();
	ScrollBar(float);

	void setValue(float);
	void setButton(sh<Button>);
	void setBounds(float, float, float, float) override;
	void addValueListener(ValueListener);
	void fireValueListeners();
private:
	int clickX = 0;
	float btnBeginX = 0;
	bool held = false;
};

