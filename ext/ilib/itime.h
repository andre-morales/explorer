#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
static inline TimePoint timenow(){
	return std::chrono::high_resolution_clock::now();
}
static inline double elapsedbetween(TimePoint stime, TimePoint etime){
	return std::chrono::duration_cast<std::chrono::microseconds>(etime - stime).count() * 0.000001;
}
static inline double elapsedsince(TimePoint stime){
	return elapsedbetween(stime, std::chrono::high_resolution_clock::now());
}
