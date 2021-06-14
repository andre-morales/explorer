#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "ilib/Math/vec4.h"
#include "ilib/Math/vec3.h"
#include "ilib/Math/vec2.h"
class Component;
class GLContext;

class GUI {
public:
	Shared<Component> root;
	Weak<Component> hoveredComponent;
	Weak<Component> focusedComponent;

	GUI(Shared<GLContext>);
	~GUI();

	void onMouseMotion(float, float);
	void onMouseButton(byte, byte, byte, float, float);
	void onKey(int, int, int, int);
	void render();
private:
	void* frameSizeListener;

	Weak<GLContext> context;

    void propagateOnMouseButton(Shared<Component>, const vec4f&, const vec3<byte>&, const vec2f&);
	void fireOnMouseButton(Shared<Component>, const vec3<byte>&, const vec2f&, const vec2f&, bool);

	void propagateOnMouseMotion(Shared<Component>, const vec4f&, const vec2f&);
	void fireOnMouseMotion(Shared<Component>, const vec2f&, const vec2f&, bool);
};
