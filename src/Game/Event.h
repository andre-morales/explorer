#pragma once
#include "ilib/types.h"
#include <initializer_list>

class Event {
public:
	uint32 id;
	uint8 argc;
	void** args;

	Event(uint32);
	Event(uint32, int8, void**);
	Event(uint32, std::initializer_list<void*>);

	static void destroy(uint8, void**);
};