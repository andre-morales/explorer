#pragma once
#include "Exception.h"
#include <string>

struct SocketException : public Exception {
	SocketException(std::string m) : Exception(m){}
	~SocketException(){}
};