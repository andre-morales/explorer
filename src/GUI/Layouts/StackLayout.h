#pragma once
#include "Layout.h"

class StackLayout : public Layout {
public:
	StackLayout() = default;
	~StackLayout() = default;
	void imp_relayout(Component&) override;
	void added(Component&) override;
};

