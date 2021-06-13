#pragma once
#include "GUI/Component.h"
class Label;
class GLContext;

class TextField : public Component {
public:
	std::string placeHolder;

	TextField();
	TextField(const std::string&);
	void setPlaceHolder(const std::string&);
	virtual void setBounds(float, float, float, float) override;
	virtual void setFont(Shared<Font>) override;
	virtual void paintComponent(GLContext&, vec2f) override;
private:
	std::shared_ptr<Label> lblText;
};
