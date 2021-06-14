#pragma once
#include "GUI/Component.h"
#include "GUI/Text.h"

class Label : public Component, public Text {
public:
	Label();
	Label(const std::string&);

	virtual void paintComponent(GLContext&, vec2f) override;
};
