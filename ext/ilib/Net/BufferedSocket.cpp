#include "BufferedSocket.h"
#include "Socket.h"
#include <iostream>
#include "SocketException.h"

BufferedSocket::BufferedSocket(un<Socket>&& s) : sock(std::move(s)){
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

uint32 BufferedSocket::write(const byte* data, uint32 len){
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

uint32 BufferedSocket::read(byte* buff, uint32 len){
     //First read all that can be read to internal buffer.
    int br = sock->read(this->buffer + writePtr, bufferSize - writePtr);
    if(br > 0){
        writePtr += br;
    }


    // Then transfer what was requested to the provided buffer.
    if(len == 0 || buff == nullptr) return 0;

    uint32 available = writePtr - readPtr;
    if(len > available) len = available;

    memcpy(buff, this->buffer + readPtr, len);
    readPtr += len;
    return len;
}

void BufferedSocket::clearup(){
    memmove(buffer, buffer + readPtr, (writePtr - readPtr));
    writePtr -= readPtr;
    readPtr = 0;
}

uint32 BufferedSocket::available(){
    return writePtr - readPtr;
}

