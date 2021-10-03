#pragma once
#include "GUI/Components/Component.h"

class GridPane : public Component {
public:
	enum Direction {
		LR_TB,
		LR_BT
	} direction = LR_TB;
	int gw = 1, gh = 1;
	float vs = 0, hs = 0;

	GridPane();
	~GridPane();

	void setGrid(int, int);
	void setSpacing(float, float);
	Shared<Component> add(Shared<Component>, int = -1) override;
	void setComponentSize(int, int, int);
	void setBounds(float, float, float, float) override;
	void relayout();

private:
	std::vector<vec2i> sizes;
};
