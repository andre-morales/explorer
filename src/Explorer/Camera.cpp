#include "Camera.h"
#include "Explorer/Explorer.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include <cmath>
#define RADS (3.14159265359/180.0)

Camera::Camera(){}

double* Camera::makeProjection(float aspect, float fov){
	projection = mat4d::perspective(aspect, fov, near, far);
	return projection.mat;
}

double* Camera::makeView(){
	view = mat4d::translation(-pos.x, -pos.y, -pos.z).rotateY(rot.y*RADS).rotateX(rot.x*RADS);
	return view.mat;
}

double* Camera::makeTransform(){
	transform = view * projection;
	return transform.mat;
}
