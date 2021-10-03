#pragma once
#include <list>
#include <functional>
#include "ilib/types.h"

class Explorer;

class Task{
public:
	typedef std::function<void()> FN_PTR;

	uint32 delay = 0;
	FN_PTR func;

	Task(FN_PTR f) : Task(0, f){}
	Task(uint32 d, FN_PTR f) : delay(d), func(f){}
};

struct ExplorerImpl{
	class Explorer& explorer;

	std::list<Task> tasks;

	ExplorerImpl(Explorer& g);
	~ExplorerImpl();

	void updateDebugText();
};

