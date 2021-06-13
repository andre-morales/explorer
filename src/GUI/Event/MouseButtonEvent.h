#pragma once
#include "ilib/types.h"
struct MouseButtonEvent {
	Component* const source;
	const byte button, action;
	const uint32 mods;
	const float sx, sy;
	const float cx, cy;

	MouseButtonEvent(Component* src, byte btn, byte act, uint32 mods, const vec2f& s, const vec2f& c):
	source(src),
	button(btn),
	action(act),
	mods(mods),
	sx(s.x),
	sy(s.y),
	cx(c.x),
	cy(c.y){}

	inline bool isLeftButton() const { return button == 0; }
	inline bool isRightButton() const { return button == 1; }
	inline bool isMiddleButton() const { return button == 2; }
	inline bool isPress() const { return action == 1; }
	inline bool isRelease() const { return action == 0; }
};
