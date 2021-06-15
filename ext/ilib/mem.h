#pragma once
#include <memory>

template<class T> using Unique = std::unique_ptr<T>;
template<class T> using Shared = std::shared_ptr<T>;
template<class T> using Weak = std::weak_ptr<T>;

template<class T, class... Args>
Unique<T> mkUnique(Args&&... args){
	return std::unique_ptr<T>(new T(args...));
}

template<class T, class... Args>
Shared<T> mkShared(Args&&... args){
	return std::make_shared<T>(args...);
}
