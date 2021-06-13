#pragma once
#include <ilib/types.h>

class Socket;

class ASocket {
public:
	static void spinReadEx(Socket&, byte*, uint32);
};
