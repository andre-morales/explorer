#pragma once
#include "Net/Packet.h"
#include "ilib/mem.h"
#include "std/string.h"

class Explorer;
struct GameImpl;

/**
 * This class only gets created when the player joins a planet. It represents
 * an instance of a running game. This class holds structures and states
 * regarding this instance and has a socket for network communication,
 * pointers to physical layers, and other states that are only available
 * in-game such as chat and pause menu. It is also responsible for holding
 * network code, these are prefixed with net_*.
 */
class Game {
	Explorer& explorer;
	un<GameImpl> impl_;
	GameImpl& impl;

public:
	string name;
	class Universe* universe = 0;
	class Planet* planet = 0;
	un<class Camera> camera;
	un<class TextLog> chatLog;
	bool chatOpen = false;
	bool gameFocused = false;

	Game(Explorer&);
	~Game();

	// Data construction/destruction
	void init();
	bool connect(string, uint16);
	void start();
	void update(double);
	void stop();
	void disconnect();
	void shutdown();
	void registerListeners();
	void unregisterListeners();
	bool areListenersRegistered();
	bool isConnected();
	bool inPlanet();

	// Net
	void net_join();

private:
	sh<class Player> player;
	class BufferedSocket* socket = nullptr;

	Packet net_inPacket;
	Packet net_outPacket;

	double lastMX, lastMY;
	sh<class Component> chatGUI, menuGUI;
	sh<class TextField> chatInputField;
	sh<class Label> chatLogLabel;
	void* keyListener = 0;
	void* charListener = 0;
	void* mouseButtonListener = 0;
	void* mouseMotionListener = 0;
	bool uiOpen = false;
	bool chatOpening = false;
	bool menuOpen = false;
	float renderDistance = 12;

	sh<Component> makePauseMenu();
	sh<Component> makeChatPanel();

	void openMenu();
	void closeMenu();
	void openChat();
	void sendChatMessage();
	void closeChat();

	bool net_receive();
	void net_process();
	void net_send();

	void net_requestChunk(uint64);
	void net_sendChatMessage();
	void log(const string&, const string&);
};
