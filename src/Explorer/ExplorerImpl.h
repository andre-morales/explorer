#pragma once
#include <list>
#include <mutex>
#include <functional>
#include "ilib/types.h"
#include "ilib/obs_unordered_map.h"

class Explorer;

class Task{
public:
	typedef std::function<void()> FN_PTR;

	uint32 delay = 0;
	FN_PTR func;

	Task(FN_PTR f) : Task(0, f){}
	Task(uint32 d, FN_PTR f) : delay(d), func(f){}
};

class ExplorerImpl {
public:
	typedef std::function<bool(std::vector<std::string>)> Command;
	class Explorer& This;

	std::unordered_map<std::string, Command> commands;
	std::list<Task> tasks;
	FILE* logFile = 0;
	std::mutex logLock;
	std::string debugText;
	bool debugTextVisible = false;

	ExplorerImpl(Explorer& g);
	~ExplorerImpl();

	void exit();
	void quitGame();
	void updateDebugText();
};

