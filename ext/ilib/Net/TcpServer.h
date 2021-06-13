#pragma once
#include "Networking.h"
class Socket;

class TcpServer {
public:
	TcpServer(int);
	~TcpServer();

	void start();
	Socket* acceptConnection();
private:
	NetSocket serverSocket;
};