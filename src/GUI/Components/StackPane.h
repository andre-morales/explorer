#pragma once
#include "GUI/Component.h"

class StackPane : public Component {
public:
	StackPane();
	~StackPane();

	Shared<Component> add(Shared<Component>) override;
	void setBounds(float, float, float, float) override;
};