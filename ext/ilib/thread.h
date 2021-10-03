#pragma once
#include <thread>

class Thread {
public:
	bool interruptFlag = false;
	std::thread thread;

    template<class Function>
	Thread(Function fun) : thread(fun){}
	~Thread();

	void join();
	void interrupt();
	bool getInterruptFlag();
};
