#pragma once
#include <cmath>
template <class T> union vec2 {
	T vec[2];
	struct { T x, y; };
	struct { T w, h; };

	vec2(){}
	vec2(T x, T y) : x(x), y(y){}

	T length() const {
		return std::sqrt(x*x + y*y);
	}

	vec2<T> operator+(T n) const {
		return {x + n, y + n};
	}
	template<class K> vec2<T> operator+(const vec2<K>& v) const {
		return {x + v.x, y + v.y};
	}
	template<class K> vec2<T> operator-(const vec2<K>& v) const {
		return {x - v.x, y - v.y};
	}
	vec2<T> operator-() const {
		return {-x, -y};
	}
	vec2<T> operator*(const T n) const {
		return {x * n, y * n};
	}
	bool operator==(const vec2<T>& other) const {
		return other.x == x && other.y == y;
	}
	bool operator!=(const vec2<T>& other) const {
		return other.x != x || other.y != y;
	}
};
using vec2i = vec2<int>;
using vec2f = vec2<float>;
using vec2d = vec2<double>;
