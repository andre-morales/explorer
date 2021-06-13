#include "Color.h"

PRNG Color::randomColorGen(0);

Color::Color()
: r(0), g(0), b(0), a(0){}

Color::Color(float r, float g, float b)
: r(r), g(g), b(b), a(1){}

Color::Color(float r, float g, float b, float a)
: r(r), g(g), b(b), a(a){}

Color Color::random(){
	auto& rcg = randomColorGen;
	float r = (rcg.gen() % 255) / 255.0;
	float g = (rcg.gen() % 255) / 255.0;
	float b = (rcg.gen() % 255) / 255.0;
	return Color(r, g, b);
}