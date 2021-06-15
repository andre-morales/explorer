#pragma once
#include "ilib/IO/Stream.h"
#include "Networking.h"
#include <ilib/types.h>

class Socket : public Stream {
public:
	NetSocket nsocket;
	Socket(NetSocket);
	Socket(const char*, int);
	~Socket();

	int write(const byte*, uint32) override;
	int read(byte*, uint32) override;
	void setBlocking(bool);
private:
    bool blocking = true;
};
