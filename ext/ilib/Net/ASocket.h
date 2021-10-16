#pragma once
#include "ilib/types.h"

class ASocket {
public:
    static void spinReadEx(class Socket&, byte*, uint32);
};
