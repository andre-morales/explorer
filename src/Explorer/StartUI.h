#pragma once
#include "GUI/Components/Component.h"

class StartUI : public Component {
public:
	StartUI(class Explorer&);
	~StartUI();

	void create();
	void setGL(unsigned int);
private:
	Explorer& explorer;
	class Label *glSelectText = 0, *glTextProps = 0;
	sh<class Toggle> toggleLogging;
	unsigned int glversion = 0;
};
