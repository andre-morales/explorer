#pragma once
#include "ilib/mem.h"
#include <functional>

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

	void initInput();
	void join();
private:
    double lastMX, lastMY;
    void* mouseMotionListener;
};
