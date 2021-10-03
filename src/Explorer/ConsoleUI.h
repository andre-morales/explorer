#pragma once
#include "Explorer/Explorer.h"
#include "GUI/Components/Component.h"

class ConsoleUI : public Component {
public:
	Shared<class Label> consoleLabel;
	Shared<class TextField> inputField;

	ConsoleUI(Explorer&);
	~ConsoleUI();

	void create();
	void setBounds(float, float, float, float) override;
	//void onKey(int, int, int, int) override;

	void log(const std::string&);
private:
	Explorer& explorer;
	Shared<Component> subRoot, consoleScroll;
};