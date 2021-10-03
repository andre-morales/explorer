#pragma once
#include "ilib/types.h"
class Component;

struct CharEvent {
	Component* const source;
	const uint32 chr;

	CharEvent(Component*, uint32);
	char getChar() const;

	constexpr bool isBackspace() const {
		return chr == 259; // GLFW_KEY_BACKSPACE;
	}
};
