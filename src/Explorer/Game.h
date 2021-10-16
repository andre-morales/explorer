#pragma once
#include "Net/Packet.h"
#include "ilib/mem.h"
#include <string>

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
	std::string name;
	class Universe* universe = 0;
	class Planet* planet = 0;
	un<class Camera> camera;
	un<class TextLog> chatLog;
	bool chatOpen = false;
	bool gameFocused = false;
	float renderDistance = 0;
	float downKiBps = 0;
	float upKiBps = 0;
	uint64 downBytesCounterFull = 0;
	uint64 upBytesCounterFull = 0;

	Game(Explorer&);
	~Game();

	// Data construction/destruction
	void init();
	bool connect(std::string, uint16);
	void start();
	void stop();
	void disconnect();
	void shutdown();
	void registerListeners();
	void unregisterListeners();
	bool areListenersRegistered();

	void update(double);
	void onSecond();
	bool isConnected();
	bool inPlanet();

	// Net
	void net_join();

private:
	sh<class Player> player;
	
	Packet net_inPacket;
	Packet net_outPacket;

	double lastMX, lastMY;
	sh<class Component> chatGUI, menuGUI;
	sh<class TextField> chatInputField;
	sh<class Label> chatLogLabel;

	bool uiOpen = false;
	bool chatOpening = false;
	bool menuOpen = false;
	
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
	void net_disconnect();
	void log(const std::string&, const std::string&);
};
