#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "Math/vec4.h"
#include "Math/vec3.h"
#include "Math/vec2.h"
class Component;
class StackPane;
class GLContext;

class GUI {
public:
	Shared<StackPane> root;
	Component* hoveredComponent;
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
	
	void propagateOnMouseMotion(Component*, const vec4f&, const vec2f&);
	void fireOnMouseMotion(Component*, const vec2f&, const vec2f&, bool);

	void propagateOnMouseButton(Shared<Component>, const vec4f&, const vec3<byte>&, const vec2f&);
	void fireOnMouseButton(Shared<Component>, const vec3<byte>&, const vec2f&, const vec2f&, bool);
};