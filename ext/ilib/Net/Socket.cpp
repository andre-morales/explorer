#include "Socket.h"
#include <winsock2.h>
#include <iostream>
#include "SocketException.h"

using namespace std;

Socket::Socket(NetSocket s) : nsocket(s){}

Socket::Socket(const char* ipaddress, int port){
	sockaddr_in server;
	server.sin_addr.s_addr = inet_addr(ipaddress);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);

	nsocket = socket(AF_INET, SOCK_STREAM, 0);
	if(nsocket == INVALID_SOCKET){
		throw SocketException(string("Could not create socket. ") + to_string(WSAGetLastError()));
	}

	if (connect(nsocket, (sockaddr*)(&server), sizeof(server)) < 0){
		throw SocketException(string("Socket connection failed. ") + to_string(WSAGetLastError()));
	}
}
Socket::~Socket(){}

int Socket::write(const byte* data, uint32 len){
	int res = send(nsocket, (const char*)data, len, 0);
	if(res < 0){
		return -1;
	}
	return res;
}

int Socket::read(byte* buff, uint32 len){
    int br = recv(nsocket, (char*)buff, len, 0);
    if(br < 0){
        if(br == SOCKET_ERROR){
            if(blocking){
                auto err = WSAGetLastError();
                throw SocketException(std::string("Socket error(" + std::to_string(err) + ")."));
            }
            return 0;
        }

        throw SocketException("Unknown socket error.");
    }
    return br;
}

void Socket::setBlocking(bool b){
    blocking = b;
    u_long mode = b?0:1;
    ioctlsocket(nsocket, FIONBIO, &mode);
}
