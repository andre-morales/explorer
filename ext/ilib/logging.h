#pragma once
#include <iostream>

inline void log(const char* m){ std::cout << m; }
inline void log(const std::string& m){ std::cout << m; }
inline void logln(const char* m){ std::cout << m << "\n"; }
inline void logln(const std::string& m){ std::cout << m << "\n"; }
inline void log(const char* c, const char* m){ std::cout << "[" << c << "] " << m << "\n"; }
inline void log(const std::string& c, const std::string& m){ std::cout << "[" << c << "] " << m << "\n"; }
