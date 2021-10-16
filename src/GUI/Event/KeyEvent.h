#pragma once
class Component;

struct KeyEvent {
	Component* const source;
	const int key, scancode, action, mods;

	KeyEvent(Component*, int, int, int, int);
	char getChar() const;

	constexpr bool isPress() const {
		return action == 1;
	}

	constexpr bool isRepeat() const {
		return action == 2;
	}

	constexpr bool isBackspace() const {
		return key == 259; // GLFW_KEY_BACKSPACE;
	}
};
