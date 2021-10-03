#include "thread.h"

Thread::~Thread(){}

void Thread::join(){
	thread.join();
}
void Thread::interrupt(){
	interruptFlag = true;
}
bool Thread::getInterruptFlag(){
	return interruptFlag;
}