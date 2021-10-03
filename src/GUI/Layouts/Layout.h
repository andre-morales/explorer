#pragma once
#include "ilib/mem.h"
class Component;

class Layout {
public:
	Component* component = nullptr;

	Layout();
	virtual ~Layout();

	void relayout();
	virtual void added(Component&);
protected:
	virtual void imp_relayout(Component&);
private:
	Layout(const Layout&) = delete;
};

