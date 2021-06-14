#pragma once
#include "ilib/mem.h"
#include <functional>
#include <string>

class Explorer;
class BufferedSocket;
class Universe;
class Player;
class Camera;

class Game {
public:
    Explorer& explorer;
	BufferedSocket* socket;
    Shared<Universe> universe;
    Shared<Player> player;
    Unique<Camera> camera;

	Game(Explorer&);
	~Game();

	bool chatOpen = false;
	std::string chatText = "";

	void sendChatMessage();
	void initInput();
	void join();
	void update();
private:
	bool chatOpening = false;

    double lastMX, lastMY;
    void* mouseButtonListener;
    void* mouseMotionListener;
    void* keyListener;
    void* charListener;
};
