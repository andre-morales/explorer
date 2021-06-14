#pragma once
#include "ilib/Math/prng.h"

struct Color {
	float r, g, b, a;

	Color();
	Color(float, float, float);
	Color(float, float, float, float);

	static Color random();
private:
	static PRNG randomColorGen;
};
