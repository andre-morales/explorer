#include "Event.h"

typedef void* VOIDPTR;

Event::Event(uint32 t) : Event(t, 0, nullptr){}

Event::Event(uint32 t, int8 c, void** a) : id(t), argc(c), args(a){}

Event::Event(uint32 t, std::initializer_list<void*> il) : id(t), argc(il.size()) {
	if(argc == 0) args = nullptr;

	args = new VOIDPTR[argc];
	int i = 0;
	for(void* ptr : il){
		args[i++] = ptr;
	}
}

void Event::destroy(uint8 argc, void** args){

}
