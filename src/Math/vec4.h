#pragma once
#include <math/vec3.h>
template<class T> union vec4 {
	T vec[4];
	struct { T r, g, b, a; };
	struct { T x, y, z, w; };
	struct { T le, to, ri, bo; };
	struct { T left, top, right, bottom; };
	struct { T x_, y_, wi, he; };

	vec4(){}
	vec4(T v) : x(v), y(v), z(v), w(v){}
	vec4(T x, T y, T z) : x(x), y(y), z(z), w(0){}
	vec4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w){}
	template<class K> vec4(const vec3<K>& v, T w){
		vec[0] = v.vec[0];
		vec[1] = v.vec[1];
		vec[2] = v.vec[2];
		vec[3] = w;
	}
	template<class K> vec4(const vec3<K>& v) : vec4(v, 0){}
	vec4<T> operator*(T s) const {
		return vec4<T>(x * s, y * s, z * s, w * s);
	}
	template<class K> vec4<T> operator+(const vec4<K>& v) const {
		return vec4<T>(x + v.x, y + v.y, z + v.z, w + v.w);
	}
	template<class K> vec4<T> operator-(const vec3<K>& v) const {
		return vec4<T>(x - v.x, y - v.y, z - v.z, w);
	}
	vec4<T> rotateX(T angle){
		double cosa = cos(angle);
		double sina = sin(angle);
		return vec4<T>(x, y*cosa + z*-sina, y*sina + z*cosa, w);
	}
	vec4<T> rotateY(T angle){
		double cosa = cos(angle);
		double sina = -sin(angle);
		return vec4<T>(x*cosa - z*sina, y, x*sina + z*cosa, w);
	}
	operator const float*() const {
		return vec;
	}
};
using vec4i = vec4<int>;
using vec4f = vec4<float>;
using vec4d = vec4<double>;
