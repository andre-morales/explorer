#pragma once
#include "GUI/Component.h"

class GridPane : public Component {
public:
	enum Orientation {
		LR_TB,
		LR_BT
	};
	Orientation orientation = LR_TB;
	int gw = 1, gh = 1;
	float vs = 0, hs = 0;

	GridPane();
	~GridPane();

	void setGrid(int, int);
	void setSpacing(float, float);
	Shared<Component> add(Shared<Component>) override;
	void setBounds(float, float, float, float) override;
	void relayout();
};
