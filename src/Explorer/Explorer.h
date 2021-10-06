#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "std/string.h"
#include "std/function.h"
#include <vector>
#include <unordered_map>

struct ExplorerImpl;

/**
 * This is the main class for the client.
 * It holds info about the states of the menus in the title screen
 * and holds pointers to other structures such as the renderer and
 * the actual game instance (Game.h).
 * Init / Shutdown / Updating and other main functions reside here.
 */
class Explorer {
	un<ExplorerImpl> impl_;
	ExplorerImpl& impl;

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
	
	Explorer();
	~Explorer();

	void init();
	void run();
	void shutdown();
	void update(double);

	void joinGame(const string&, uint16, const string&);
	void quitGame();
	void localCommand(const string&);
	void openMainMenu();

	void regTask(function<void()>);
	template <class Class, class... Args>
	void regTask(void(Class::* f)(Args...), Class* t, Args&&... a) {
		regTask(std::bind(f, t, a...));
	}
	void regCmd(string, function<void(std::vector<string>)>);

	void log(const string&);
	void log(const string&, const string&);

private:
	typedef function<void(std::vector<string>)> Command;
	std::unordered_map<string, Command> commands;
	Unique<class TextLog> consoleLog;
	Weak<class ConsoleUI> console;

	void begin();
	void registerBlocks();
	void imp_joinGame(const string&, uint16, const string&);
	void imp_quitGame();
	void imp_localCommand(const string&);
	void imp_openMainMenu();
	void updateDebugText();
};

#define E_BUILD_YEAR "21"
#define E_BUILD_MONTH "10"
#define E_BUILD_DAY "06"
#define E_BUILD_INDEX "000"
#define E_BUILD_T_STR E_BUILD_YEAR "-" E_BUILD_MONTH "-" E_BUILD_DAY "_" E_BUILD_INDEX

#define E_VERSION_MAJOR "0"
#define E_VERSION_MINOR "3"
#define E_VERSION_REVISION "0"
#define E_VERSION_STR E_VERSION_MAJOR "." E_VERSION_MINOR "." E_VERSION_REVISION

#ifdef RELEASE
	#define E_BUILD_STR "r" E_BUILD_T_STR
#elif DEBUG
	#define E_BUILD_STR "Debug"
#else
	#define E_BUILD_STR "DevB" E_BUILD_T_STR
#endif
