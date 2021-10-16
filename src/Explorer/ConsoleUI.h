#pragma once
#include "Explorer/Explorer.h"
#include "GUI/Components/Component.h"

class ConsoleUI : public Component {
public:
	sh<class Label> consoleLabel;
	sh<class TextField> inputField;

	ConsoleUI(Explorer&);
	~ConsoleUI();

	void create();
	void setBounds(float, float, float, float) override;

	void log(const std::string&);
private:
	std::vector<std::string> cmdHistory;
	int cmdNav = -1;
	Explorer& explorer;
	sh<Component> subRoot, consoleScroll;
};