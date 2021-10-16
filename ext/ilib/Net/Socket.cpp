#include "Socket.h"
#include <WS2tcpip.h>
#include <iostream>
#include "SocketException.h"

std::string wsaErrorStr(int err){
	switch(err){
	default:
		return std::to_string(err);
	}
}

std::string getWSAErrorStr(){
	return wsaErrorStr(WSAGetLastError());
}

Socket::Socket(uintptr socket) : handle(socket){}

Socket::Socket(const char* ipaddress, int port){
	sockaddr_in server;
	inet_pton(AF_INET, ipaddress, &server.sin_addr.s_addr);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	handle = socket(AF_INET, SOCK_STREAM, 0);
	if(handle == INVALID_SOCKET){
		throw SocketException("Could not create socket. " + getWSAErrorStr());
	}

	if (connect(handle, (sockaddr*)(&server), sizeof(server)) < 0){
		throw SocketException("Socket connection failed. " + getWSAErrorStr());
	}
}
Socket::~Socket(){
	forceClose();
}

uint32 Socket::write(const byte* data, uint32 len){
	if(!len) return 0;

	int res = send(handle, (const char*)data, len, 0);
	if(res < 0){
		int err = WSAGetLastError();
		if(blocking || err != WSAEWOULDBLOCK){
			throw SocketException("Socket write error. " + wsaErrorStr(err));
		}
		throw SocketException(SocketException::BUFFER_FULL);
	}
	return res;
}

uint32 Socket::read(byte* buff, uint32 len){
	int br = recv(handle, (char*)buff, len, 0);
	if(br < 0){
		int err = WSAGetLastError();
		if(blocking || err != WSAEWOULDBLOCK){
			throw SocketException("Socket read error. " + wsaErrorStr(err));
		}
		return 0;
	}
	return br;
}

uint32 Socket::toRead(){
	u_long data;
	ioctlsocket(handle, FIONREAD, &data);
	return data;
}

void Socket::setBlocking(bool b){
	blocking = b;
	u_long mode = b?0:1;
	ioctlsocket(handle, FIONBIO, &mode);
}
void Socket::forceClose () {
	closesocket(handle);
}