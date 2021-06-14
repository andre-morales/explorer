#pragma once
#include <cmath>
template <class T> union vec3 {
	T vec[3];
	struct { T x, y, z; };

	vec3(){}
	vec3(T x, T y, T z) : x(x), y(y), z(z){}
	template <class K> vec3(const vec3<K>& other){
		x = other.x;
		y = other.y;
		z = other.z;
	}

	T length() const {
		return std::sqrt(x*x + y*y + z*z);
	}
	vec3<T> rotateX(T angle){
		double cosa = cos(angle);
		double sina = sin(angle);
		return vec3<T>(x, y*cosa + z*-sina, y*sina + z*cosa);
	}
	vec3<T> rotateY(T angle){
		double cosa = cos(angle);
		double sina = -sin(angle);
		return vec3<T>(x*cosa - z*sina, y, x*sina + z*cosa);
	}

	vec3<T> operator+(T n) const {
		return {x + n, y + n, z + n};
	}
	template<class K> vec3<T> operator+(const vec3<K>& v) const {
		return {x + v.x, y + v.y, z + v.z};
	}
	template<class K> vec3<T> operator-(const vec3<K>& v) const {
		return {x - v.x, y - v.y, z - v.z};
	}
	vec3<T> operator-() const {
		return {-x, -y, -z};
	}
	vec3<T> operator*(const T n) const {
		return {x * n, y * n, z * n};
	}
	vec3<T> operator/(const T n) const {
		return {x / n, y / n, z / n};
	}
	bool operator!=(const vec3<T> other){
		return other.x != x || other.y != y || other.z != z;
	}
};
using vec3i = vec3<int>;
using vec3f = vec3<float>;
using vec3d = vec3<double>;