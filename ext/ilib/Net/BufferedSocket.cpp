#include "BufferedSocket.h"
#include "Socket.h"
#include <winsock2.h>
#include <iostream>
#include "SocketException.h"

using namespace std;

BufferedSocket::BufferedSocket(Socket* s) : sock(s){
	writePtr = 0;
	readPtr = 0;
}
BufferedSocket::~BufferedSocket(){}

void BufferedSocket::init(uint32 size){
    initialized = true;
    this->bufferSize = size;
    if(size){
        this->buffer = new byte[size];
    }
    sock->setBlocking(false);
}

int BufferedSocket::write(const byte* data, int len){
	return sock->write(data, len);
}

std::string tostr(byte* ptr, int len){
	std::string a("[");
	for(int i = 0; i < len; i++){
		a += std::to_string(ptr[i]);
		if(i < len - 1){
			a += std::string(", ");
		}

	}
	a += "]";
	return a;
}

int BufferedSocket::read(byte* buff, int len){
     //First read all that can be read to internal buffer.
    int br = sock->read(this->buffer + writePtr, bufferSize - writePtr);
    writePtr += br;

    // Then transfer what was requested to the provided buffer.
    if(len <= 0) return 0;

    int available = writePtr - readPtr;
    if(len > available) len = available;

    memcpy(buff, this->buffer + readPtr, len);
    readPtr += len;
    return len;
}

int BufferedSocket::available(){
    return writePtr - readPtr;
}
