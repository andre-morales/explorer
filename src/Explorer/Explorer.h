#pragma once
#include "ilib/mem.h"
#include <vector>

class Instance;
class Renderer;
class GUI;
class Button;
class Game;
class Explorer {
public:
	enum Screen {
		OFF, START_SCREEN, MAIN_MENU
	};

	Explorer();
	~Explorer();

	Screen screen = Screen::OFF;
    Unique<Instance> explorerInstance;
	Unique<Game> gameInstance;
	Unique<Renderer> renderer;

	float fov = 90;
	std::vector<Shared<GUI>> attachedGUIs;

	void init();
	void run();
	void update();
	Shared<GUI> createMainMenuGUI();
	Shared<GUI> createStartGUI();
	void shutdown();
private:
	void handleEventsForGUIs();

	Shared<Button> gui_button(const char*);
};
