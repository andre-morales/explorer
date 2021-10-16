#pragma once
#include "Layout.h"

class SplitLayout : public Layout {
public:
	enum Orientation {
		HORIZONTAL, VERTICAL
	};

	Orientation orientation = HORIZONTAL;
	float value = 0.5;

	SplitLayout() = default;
	SplitLayout(Orientation, float);
	~SplitLayout() = default;
	void imp_relayout(Component&) override;
	void added(Component&) override;
};

