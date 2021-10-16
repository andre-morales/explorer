#include "Console.h"
#include <iostream>

void Console::foreground(char color){
	std::cout << "\033[1;31m";
}

bool Console::colorsSupported () {
	return false;
}