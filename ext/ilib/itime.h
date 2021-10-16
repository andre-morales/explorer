#include <chrono>

typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
typedef std::chrono::duration<std::chrono::high_resolution_clock> TimeDuration;

static inline TimePoint timenow(){
	return std::chrono::high_resolution_clock::now();
}
static inline double elapsedbetween(TimePoint stime, TimePoint etime){
	return std::chrono::duration_cast<std::chrono::microseconds>(etime - stime).count() * 0.000001;
}
template <class T>
static inline double time_as_millis(std::chrono::duration<T> time){
	return std::chrono::duration_cast<std::chrono::microseconds>(time).count() * 0.000001;
}
static inline double elapsedsince(TimePoint stime){
	return elapsedbetween(stime, std::chrono::high_resolution_clock::now());
}
