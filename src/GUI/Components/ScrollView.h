#pragma once
#include "GUI/Components/Component.h"

class ScrollView : public Component {
public:
	float scrollX = 0, scrollY = 0;

	ScrollView();
	~ScrollView();

	Shared<Component> add(Shared<Component>, int = -1) override;
	void setBounds(float, float, float, float) override;
	virtual void paintChildren(GLContext&, vec2f) override;
};
