#pragma once
#include "ilib/Net/Socket.h"
#include "ilib/mem.h"
#include "ilib/thread.h"
#include <vector>
#include <string>
#include <mutex>
#include <queue>
class Instance;
class TcpServer;
class Client;

namespace YAML {
	class Node;
}

class Server {
public:
	YAML::Node* config = 0;
	un<Instance> explorerInstance;
	un<TcpServer> svSocket;
	struct GenRequest {
		uint64 id;
		byte style;
		class Client* client;
	};
	Thread* clientListenerThread = 0;
	Thread* worldGeneratorThread = 0;
	std::mutex worldGenLock;
	std::condition_variable worldGenCV;
	std::queue<struct GenRequest*> worldGenQueue;

	std::vector<Client*> clients;
	Server();
	~Server();

	void init();
	void loadConfig();
	void run();
	void stop();

private:
	void clientListenerFn();
	void worldGeneratorFn();
	void clientThreadFn(Client*);
	void handleClientPacket(Client&, class Packet&);
	void handleClientOutQueue(Client&);
	void net_sendChatMessage(const std::string&);
	//void net_spawnPlayer(const std::string&);
	void broadcastPacket(sh<Packet>&);
	void createChunkPacket(Client&, Packet&, class Chunk*);
	void createChatMessagePacket(Packet&, const std::string&);
	bool clientCommand(Client&, const std::string&);
};

