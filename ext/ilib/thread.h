#pragma once
#include <thread>

class Thread {
public:
	bool interruptFlag = false;
	std::thread thread;

    
	Thread(std::thread&& t) : thread(std::move(t)){}
	template<class F>
	Thread(F fun) : thread(fun){}
	~Thread();

	void join();
	void interrupt();
	void interruptAndWait();
	bool getInterruptFlag();
};
