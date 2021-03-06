#pragma once
#include "ilib/Math/vec3.h"

class Entity {
public:
	vec3d speed = {0, 0, 0};
	vec3d pos = {0, 0, 0};
	double rotX, rotY, rotZ;

	Entity();
	~Entity();
private:
    Entity(const Entity&);
};
