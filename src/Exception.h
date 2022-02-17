#pragma once
#include <exception>
#include <string>

struct Exception : public std::exception {
	const std::string message;

	Exception() : Exception("(Exception message)"){}
	Exception(const std::string& m)
	: std::exception("nice"),
	message(m){
		
	}
	virtual ~Exception(){}

	const char* what() const noexcept override {
		return message.c_str();
	}
};