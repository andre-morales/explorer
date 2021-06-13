#pragma once
#include "ilib/types.h"
struct MouseMotionEvent {
	Component* const source;
	const uint8 action;
	const float cx, cy; // Component Coordinates.
	const float sx, sy; // Screen Coordinates.

	MouseMotionEvent(Component* src, uint8 act, const vec2f& s, const vec2f& c)
	: source(src), action(act), cx(c.x), cy(c.y), sx(s.x), sy(s.y){}

	inline bool isMoving() const { return action == 0; }
	inline bool isEntering() const { return action == 1; }
	inline bool isLeaving() const { return action == 2; }
};
