#pragma once
#include "ilib/Math/vec2.h"
#include "ilib/Math/vec3.h"
#include "ilib/Math/mat4.h"

class Camera {
public:
    float fov = 90, aspect = 4.0/3.0;
	float near = 0.01, far = 2000.0;
	vec3d pos = vec3d(0, 0, 0);
	vec2f rot = vec2f(0, 0);
	mat4f projection, rotatedView, view, transform;

    Camera();
	void makeProjection();
	void makeRotatedView();
	void makeView();
	void makeTransform();

private:
    Camera(const Camera&) = delete;
};
