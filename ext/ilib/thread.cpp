#include "thread.h"

Thread::~Thread(){}

void Thread::join(){
	thread.join();
}
void Thread::interrupt(){
	interruptFlag = true;
}
void Thread::interruptAndWait(){
	interrupt();
	join();
}
bool Thread::getInterruptFlag(){
	return interruptFlag;
}