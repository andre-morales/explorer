#pragma once
#include "Networking.h"
#include "ilib/IO/Stream.h"
#include "ilib/mem.h"
#include "ilib/types.h"

class Socket;

class BufferedSocket : public Stream {
public:
	un<Socket> sock;
	byte* buffer = 0;
	uint32 bufferSize = 0;
	uint32 readPtr = 0;
	uint32 writePtr = 0;

	BufferedSocket(un<Socket>&&);
	~BufferedSocket();

	void init(uint32);
	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	void clearup();
	uint32 available();

private:
	BufferedSocket(const BufferedSocket&) = delete;
    bool initialized = false;
};
