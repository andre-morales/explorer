#pragma once
#include "GUI/Component.h"

class Label : public Component {
public:
	Label();
	Label(const std::string&);
	virtual void paintComponent(GLContext&, vec2f) override;
};
