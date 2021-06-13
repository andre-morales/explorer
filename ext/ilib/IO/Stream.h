#pragma once
#include <ilib/types.h>

class Stream {
public:
    Stream();
    virtual ~Stream();
    virtual int write(const byte*, uint32);
    virtual int read(byte*, uint32);
};
