#pragma once
#include "GUI/Components/Component.h"
#include "GUI/Text.h"

class GLContext;

class TextField : public Component, public Text {
public:
	TextField();
	TextField(const std::string&);
	TextField(const TextField&) = delete;
	~TextField();

	virtual void setBounds(float, float, float, float) override;
	virtual void paintComponent(GLContext&, vec2f) override;
};
