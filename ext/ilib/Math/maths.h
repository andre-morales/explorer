#pragma once
namespace Math {
	template<class T>
	constexpr T clamp(const T& val, const T& min, const T& max){
		return ((val < min)?min:(val > max)?max:val);
	}

	template<class T>
	constexpr T min(const T& a, const T& b){
		return (a < b) ? a:b;
	}
}