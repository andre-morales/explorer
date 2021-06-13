#pragma once
#include <Math/vec2.h>
#include <Math/vec3.h>
#include <Math/mat4.h>
class Explorer;

class Camera {
public:
	float near = 0.01, far = 2000.0;
	vec3d pos = vec3d(0, 0, 0);
	vec2f rot = vec2f(0, 0);
	mat4d projection, view, transform;

    Camera();
	double* makeProjection(float, float);
	double* makeView();
	double* makeTransform();

private:
    Camera(const Camera&) = delete;
};
