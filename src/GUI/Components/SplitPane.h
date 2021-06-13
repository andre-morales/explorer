#pragma once
#include "GUI/Component.h"

class SplitPane : public Component {
public:
	enum Orientation {
		HORIZONTAL, VERTICAL
	};

	Orientation orientation = HORIZONTAL;
	float value = 0.5;

	SplitPane();
	~SplitPane();

	Shared<Component> add(Shared<Component>) override;
	void setBounds(float, float, float, float) override;
	void setOrientation(Orientation);
	void setValue(float);
private:
	void relayout();
};