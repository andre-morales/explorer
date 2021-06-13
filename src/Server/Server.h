#pragma once
#include <ilib/mem.h>

class Instance;
class TcpServer;

class Server {
public:
	Unique<Instance> explorerInstance;
	Unique<TcpServer> svSocket;

	Server();
	~Server();

	void init();
	void run();
};
