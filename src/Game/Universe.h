#pragma once
#include <vector>
#include "ilib/mem.h"
class Planet;
class Instance;

class Universe {
public:
    Instance* instance;
    std::vector<Shared<Planet>> planets;

	Universe(Instance*);
	~Universe();
};
