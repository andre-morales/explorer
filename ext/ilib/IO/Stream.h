#pragma once
#include "ilib/types.h"

class Stream {
public:
    Stream();
    virtual ~Stream();
    virtual uint32 write(const byte*, uint32);
    virtual uint32 read(byte*, uint32);
    virtual uint32 toRead();
    virtual uint32 toWrite();
    virtual uint32 readInto(Stream*, uint32);
    virtual uint32 writeFrom(Stream*, uint32);
};
