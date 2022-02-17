#pragma once
#include <exception>
#include <string>

struct IException : public std::exception {
	const std::string message;

	IException();
	IException(const std::string&);
	virtual ~IException();

	const char* what() const noexcept override;
};

