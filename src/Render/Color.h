#pragma once
class PRNG;

class Color {
public:
	float r, g, b, a;

	Color();
	Color(float, float, float);
	Color(float, float, float, float);

	static Color random();
private:
	static PRNG* randomColorGen;
};
