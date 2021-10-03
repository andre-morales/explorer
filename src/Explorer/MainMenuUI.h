#pragma once
#include "Explorer/Explorer.h"
#include "GUI/Components/Component.h"

class MainMenuUI : public Component {
public:
	MainMenuUI(Explorer&);
	~MainMenuUI();

	void create();
private:
	Explorer& explorer;
};
