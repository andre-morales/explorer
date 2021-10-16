#include "Camera.h"
#include <cmath>
#define RADS (3.14159265359/180.0)

Camera::Camera(){}

void Camera::makeProjection(){
	projection = mat4f::perspective(aspect, fov, near, far);
}

void Camera::makeRotatedView() {
	rotatedView = mat4f::rotationY(rot.y*RADS).rotateX(rot.x*RADS);
}

void Camera::makeView(){
	view = mat4f::translation(-pos.x, -pos.y, -pos.z) * rotatedView;
}

void Camera::makeTransform(){
	transform = view * projection;
}
