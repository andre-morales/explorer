#include "thread.h"

Thread::~Thread(){
	interruptAndWait();
}

void Thread::join(){
	thread.join();
}
void Thread::interrupt(){
	interruptFlag = true;
}
void Thread::interruptAndWait(){
	interrupt();
	if(thread.joinable()) join();
}
bool Thread::getInterruptFlag(){
	return interruptFlag;
}