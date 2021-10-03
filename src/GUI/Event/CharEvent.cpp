#include "CharEvent.h"
#include "GLFW/glfw3.h"

CharEvent::CharEvent(Component* src, uint32 chr) : source(src), chr(chr){}

char CharEvent::getChar() const {
    return chr;
	/*const char* kn = glfwGetKeyName(key, scancode);
	if(kn != nullptr){
		return kn[0];
	}
	switch(key){
		case GLFW_KEY_SPACE: return ' ';
		default: return 0;
	}*/
}
