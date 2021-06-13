template<class T>
constexpr T clamp(const T& val, const T& min, const T& max){
	return ((val < min)?min:(val > max)?max:val);
}
