#pragma once
#include "Networking.h"
#include "ilib/types.h"
class Socket;

class TcpServer {
public:
	TcpServer(int);
	~TcpServer();

	void start();
	Socket* acceptConnection();
	void close();
private:
	uintptr serverSocket;
};