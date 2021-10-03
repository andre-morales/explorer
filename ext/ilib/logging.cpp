#include "logging.h"
#include <iostream>

void log(const char* msg){
	std::cout << msg;
}
void logln(const char* msg){
	std::cout << msg << "\n";
}

void log(const char* cls, const char* msg){
	std::cout << "[" << cls << "] " << msg << "\n";
}

void dbg(const char* cls, const char* msg){
	std::cout << "[" << cls << "] " << msg << "\n";
}