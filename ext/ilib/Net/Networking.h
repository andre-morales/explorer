#pragma once
struct WSAData;

namespace Net {
	extern bool initialized;
	extern WSAData* wsa;

	void init();
	void release();
}