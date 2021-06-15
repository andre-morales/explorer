#pragma once
#include "ilib/Net/Socket.h"
#include <ilib/mem.h>
#include <thread>
#include <vector>

class Instance;
class TcpServer;

class Server {
public:
	Unique<Instance> explorerInstance;
	Unique<TcpServer> svSocket;
    struct ServerClient {
        std::thread* thread;
        Socket* socket;
        std::string name;
    };
    std::vector<ServerClient*> clients;

	Server();
	~Server();

	void init();
	void run();
};
