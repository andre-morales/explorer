#pragma once
#include "ilib/Net/Socket.h"
#include "ilib/mem.h"
#include "ilib/thread.h"
#include <vector>
#include <string>
#include <mutex>
#include <list>
class Instance;
class TcpServer;
class Client;
class Server {
public:
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
	std::list<struct GenRequest*> worldGenQueue;

	std::vector<Client*> clients;
	Server();
	~Server();

	void init();
	void run();
	void stop();
private:
	void clientListenerFn();
	void worldGeneratorFn();
	void clientThreadFn(Client*);
	void handleClientPacket(Client&, Socket*, class Packet&);
	void handleClientOutQueue(Client&);
	void net_sendChatMessage(const std::string&);
	void broadcastPacket(sh<Packet>&);
	void createChunkPacket(Packet&, class Chunk*);
	void createChatMessagePacket(Packet&, const std::string&);
};

