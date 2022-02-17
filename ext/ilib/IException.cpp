#include "IException.h"

IException::IException() : IException("(Exception message)") {}
IException::IException(const std::string& m)
: std::exception("nice"), message(m) {}
IException::~IException() {}

const char* IException::what() const noexcept {
	return message.c_str();
}