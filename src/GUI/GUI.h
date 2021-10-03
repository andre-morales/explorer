#pragma once
#include "ilib/mem.h"
#include "ilib/types.h"
#include "ilib/Math/vec4.h"
#include "ilib/Math/vec3.h"
#include "ilib/Math/vec2.h"
#include <string>
class Component;
class GLContext;

class GUI {
	Weak<GLContext> context;

public:
	Shared<Component> root;
	Weak<Component> hoveredComponent;
	Weak<Component> focusedComponent;
	Component* focusedComponentPtr;

	GUI(Shared<GLContext>);
	~GUI();

	void render();
	void giveFocus(Shared<Component>);
	void releaseFocus(Component*);

	void onMouseMotion(float, float);
	void onMouseButton(byte, byte, byte, float, float);
	virtual void onKey(int, int, int, int);
	virtual void onChar(uint32);

	// Mirror functions of root component
	void add(Shared<Component>, int = -1);
	Shared<Component> remove(const std::string&);
	void setVisible(bool);
	bool isVisible();
	void setBackground(bool);
	virtual void setBounds(float, float, float, float);
	vec4f getBounds();
	void registerListeners();
	void unregisterListeners();
private:
	GUI(const GUI&) = delete;

	void *frameSizeListener, *mouseButtonListener, *mouseMotionListener;
	void *keyListener, *charListener;

    void propagateOnMouseButton(Shared<Component>, const vec4f&, const vec3<byte>&, const vec2f&);
	void fireOnMouseButton(Shared<Component>, const vec3<byte>&, const vec2f&, const vec2f&, bool);

	void propagateOnMouseMotion(Shared<Component>, const vec4f&, const vec2f&);
	void fireOnMouseMotion(Shared<Component>, const vec2f&, const vec2f&, bool);
};
