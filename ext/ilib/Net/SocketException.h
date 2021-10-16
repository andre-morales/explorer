#pragma once
#include "Exception.h"
#include <string>

struct SocketException : public Exception {
	enum ErrorCode : unsigned char {
		UNKNOWN,
		BUFFER_FULL
	} code = UNKNOWN;

	SocketException(ErrorCode c) : code(c){}

	SocketException(const std::string& m)
	: Exception(m){
		
	}
};
