#pragma once
#include "Exception.h"
#include <string>

struct SocketException : public Exception {
	SocketException(const std::string& m)
	: Exception(m){
		
	}
};
