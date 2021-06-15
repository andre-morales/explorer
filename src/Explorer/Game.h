#pragma once
#include "ilib/mem.h"
#include <functional>
#include <string>
#include <queue>

class Explorer;
class Universe;
class Player;
class Camera;
class GUI;
class BufferedSocket;
class Packet;

class Game {
public:
    Explorer& explorer;
	BufferedSocket* socket;
    Shared<Universe> universe;
    Shared<Player> player;
    Unique<Camera> camera;

    bool uiOpen = false;
	bool menuOpen = false;
	bool chatOpen = false;
	std::string name;
	std::string chatText;
    std::deque<std::string> chatHistory;

	Game(Explorer&);
	~Game();

	void init();
	void update();

	Shared<GUI> makePauseMenuGUI();

	void openMenu();
	void closeMenu();
	void openChat();
	void closeChat();
	Packet* readNetPacket();
	void processNetPacket(Packet&);
	void sendChatMessage();
	void join();

private:
	bool chatOpening = false;
    double lastMX, lastMY;
    GUI* menuGUI = nullptr;
    void* mouseButtonListener;
    void* mouseMotionListener;
    void* keyListener;
    void* charListener;
};
