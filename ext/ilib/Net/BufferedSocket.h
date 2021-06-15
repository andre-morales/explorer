#pragma once
#include "Networking.h"
#include <ilib/types.h>

class Socket;

class BufferedSocket {
public:
	Socket* sock;
	byte* buffer = nullptr;
	uint32 bufferSize = 0;
	uint32 readPtr = 0;
	uint32 writePtr = 0;

	BufferedSocket(Socket*);
	~BufferedSocket();

	void init(uint32);
	int write(const byte*, int);
	int read(byte*, int);
	void clearup();
	uint32 available();

private:
    bool initialized;
};
