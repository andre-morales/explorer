#include "Render/Color.h"
#include "ilib/Math/prng.h"

PRNG* Color::randomColorGen(nullptr);

Color::Color()
: r(0), g(0), b(0), a(0){}

Color::Color(float r, float g, float b)
: r(r), g(g), b(b), a(1){}

Color::Color(float r, float g, float b, float a)
: r(r), g(g), b(b), a(a){}

Color Color::random(){
	if (!randomColorGen) randomColorGen = new PRNG(0);
	auto& rcg = *randomColorGen;
	float r = (rcg.gen() % 255) / 255.0;
	float g = (rcg.gen() % 255) / 255.0;
	float b = (rcg.gen() % 255) / 255.0;
	return Color(r, g, b);
}