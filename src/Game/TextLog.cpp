#include "TextLog.h"
#include <sstream>

TextLog::TextLog(){}

void TextLog::addLine(const std::string& str){
	container.emplace_back(str);
}

std::string TextLog::asString(){
	std::stringstream ss;
	for(const std::string& str : container){
		ss << str << "\n";
	}
	return ss.str();
}

std::string& TextLog::getLine(int n){
	return container[n];
}

unsigned int TextLog::lines(){
	return container.size();
}
