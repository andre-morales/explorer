#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include <functional>
#include <vector>

/*
 This is the main class for the Explorer client.

 It holds info about the states of the menus in the title screen
 and holds pointers to other fundamental structures such as the renderer and
 the actual game instance (Game.h).
 Init / Shutdown / Updating and other main functions reside here.
 */
class Explorer {
	/* Internal pointer to implementation. */
	un<class ExplorerImpl> impl_; 
	/* Private reference to implementation. This contains fields and methods
	that sould not be acessible to others. */
	ExplorerImpl& impl;

	/* Line by line history of the console. */
	Unique<class TextLog> consoleLog;

	/* Console GUI component. */
	Weak<class ConsoleUI> console;

public:
	enum Screen {
		OFF, START_SCREEN, MAIN_MENU
	};

	Screen screen = Screen::OFF;

	Unique<class Instance> explorerInstance;
	Unique<class Renderer> renderer;
	Unique<class ExplorerGUI> ui;
	Unique<class Game> gameInstance;

	float fov = 90;
	int fps = 0;
	float frameTime = 0;
	float swapTime = 0;
	bool loggingEnabled = false;

	Explorer();
	~Explorer();

	void init();
	void run();
	void shutdown();
	void update(double);

	void exit();
	void joinGame(const std::string&, uint16, const std::string&);
	void quitGame();
	void localCommand(const std::string&);
	void openMainMenu(uint32);

	void regTask(std::function<void()>);
	template <class Class, class... Args>
	void regTask(void(Class::* f)(Args...), Class* t, Args&&... a) {
		regTask(std::bind(f, t, a...));
	}
	void regCmd(std::string, std::function<bool(std::vector<std::string>)>);

	void log(const std::string&);
	void log(const std::string&, const std::string&);

private:
	void createLoggingFile();
	void begin();
	void registerBlocks();
	void registerCommands();

	void imp_joinGame(const std::string&, uint16, const std::string&);
	void imp_localCommand(const std::string&);
	void imp_openMainMenu(uint32);
};