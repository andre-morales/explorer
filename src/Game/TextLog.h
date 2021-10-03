#pragma once
#include <vector>
#include <string>

class TextLog {
public:
	std::vector<std::string> container;

	TextLog();
	void addLine(const std::string&);
	std::string& getLine(int);
	std::string asString();
	unsigned int lines();
};
