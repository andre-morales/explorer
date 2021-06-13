#pragma once
#include <exception>
#include <string>

struct Exception : public std::exception {
	std::string message;

	Exception(){};
	Exception(std::string m) : message(m){

	};
	~Exception(){}

	const char* what() const noexcept override {
		return message.c_str();
	}
};