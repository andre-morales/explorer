#pragma once
#include "ilib/types.h"

class Stream {
public:
    Stream(){}
    virtual ~Stream(){}
    virtual uint32 write(const byte*, uint32) = 0;
    virtual uint32 read(byte*, uint32) = 0;
    virtual uint32 available() = 0;
};
