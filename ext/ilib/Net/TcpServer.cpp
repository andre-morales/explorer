#include "TcpServer.h"
#include "Socket.h"
#include <winsock2.h>
#include "Exception.h"

TcpServer::TcpServer(int port){
	sockaddr_in server;
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(serverSocket == INVALID_SOCKET){
		throw Exception("Could not create socket.");
	}	

	int result = bind(serverSocket, (sockaddr*)(&server), sizeof(server)); 
	if(result == SOCKET_ERROR){
		printf("Bind failed with error code : %d", WSAGetLastError());
	}
}
TcpServer::~TcpServer(){}

void TcpServer::start(){
	listen(serverSocket, 3);
}

Socket* TcpServer::acceptConnection(){
	sockaddr_in client;
	int c = sizeof(sockaddr_in);
	NetSocket sock = accept(serverSocket, (sockaddr*)(&client), &c);
	if (sock == INVALID_SOCKET){
		printf("accept failed with error code : %d" , WSAGetLastError());
	}
	return new Socket(sock);
}

