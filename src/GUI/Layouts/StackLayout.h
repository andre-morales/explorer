#pragma once
#include "Layout.h"

class StackLayout : public Layout {
public:
	StackLayout();
	~StackLayout();

	void imp_relayout(Component&) override;
	void added(Component&) override;
};

