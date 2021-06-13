#include "KeyEvent.h"
#include <GLFW/glfw3.h>

KeyEvent::KeyEvent(Component* src, int k, int s, int a, int m)
: source(src), key(k), scancode(s), action(a), mods(m){}


char KeyEvent::getChar() const {
	const char* kn = glfwGetKeyName(key, scancode);
	if(kn != nullptr){
		return kn[0];	
	}
	switch(key){
		case GLFW_KEY_SPACE: return ' ';
		default: return 0;
	}
}
