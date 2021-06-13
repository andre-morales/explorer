#pragma once

typedef unsigned long long int NetSocket;

struct WSAData;

namespace Net {
	extern bool initialized;
	extern WSAData* wsa;

	void init();
	void release();
}