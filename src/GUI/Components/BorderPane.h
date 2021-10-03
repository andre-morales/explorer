#pragma once
#include "GUI/Components/Component.h"
#include "GUI/Orientation.h"

class BorderPane : public Component {
public:
	std::vector<Orientation> positions;
	std::vector<vec2f> growRules;

	BorderPane();
	~BorderPane();

	Shared<Component> add(Shared<Component>, int = -1) override;
	void setOrientation(int, Orientation);
	void setGrowRule(int, float, float);
	void setBounds(float, float, float, float) override;
	void relayout();
};
