#pragma once
#include "ilib/Net/Networking.h"
#include "ilib/IO/Stream.h"
#include "ilib/types.h"

/* Abstraction for the underlying socket systems.
   This class provides no extra buffering.
   Every call maps 1:1 to the native system. */
class Socket : public Stream {
public:
	Socket(uintptr);
	Socket(const char*, int);
	~Socket();

	uint32 write(const byte*, uint32) override;
	uint32 read(byte*, uint32) override;
	uint32 toRead() override;
	void setBlocking(bool);
	void forceClose();
private:
	uintptr handle;
    bool blocking = true;
};
