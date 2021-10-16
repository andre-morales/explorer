#include "Renderer.h"
#include <cmath>
#define RADS (3.14159265359/180.0)


/*
static constexpr float crosshair_verts[] = {
	-0.002, -0.022,
	 0.002, -0.022,
	 0.002,  0.022,
	-0.002,  0.022,

	-0.022, -0.002,
	 0.022, -0.002,
	 0.022,  0.002,
	-0.022,  0.002
};
static constexpr float vignette_verts[] = {
	-1, -1,
	-1,  1,
	 1,  1,
	 1, -1,
};
static constexpr float fog_diamond[] = {
	// Top
	0.0, 1.0, 0.0,
	-.5, 0.0, -.5,
	0.5, 0.0, -.5,

	0.0, 1.0, 0.0,
	-.5, 0.0, 0.5,
	-.5, 0.0, -.5,

	0.0, 1.0, 0.0,
	0.5, 0.0, -.5,
	0.5, 0.0, 0.5,

	0.0, 1.0, 0.0,
	0.5, 0.0, 0.5,
	-.5, 0.0, 0.5,

	// Bottom
	0.0, -1., 0.0,
	0.5, 0.0, -.5,
	-.5, 0.0, -.5,

	0.0, -1., 0.0,
	-.5, 0.0, -.5,
	-.5, 0.0, 0.5,

	0.0, -1., 0.0,
	0.5, 0.0, 0.5,
	0.5, 0.0, -.5,

	0.0, -1., 0.0,
	-.5, 0.0, 0.5,
	0.5, 0.0, 0.5,
};
static constexpr unsigned char mod(signed int x, unsigned char m){ return (x % m + m) % m; }*/

constexpr void interleave(byte* output, uintptr size, const void* in1, uint8 size1, const void* in2, uint8 size2) {
	byte* pntr = output;
	byte* input1 = (byte*)in1;
	byte* input2 = (byte*)in2;

	while(pntr < output + size) {
		memcpy(pntr, input1, size1);
		pntr += size1;
		input1 += size1;

		memcpy(pntr, input2, size2);
		pntr += size2;
		input2 += size2;
	}
}
/*static constexpr const char* __FACES[] = {"top", "bottom", "west", "east", "front", "back"};
static constexpr const char* __FACING[] = {	"north", "east", "south", "west" };
static constexpr const char* getFaceName(unsigned char face){ return __FACES[face]; }
static constexpr const char* getFacingDirection(double rx){ return __FACING[mod(floor(rx/90+0.5), 4)]; }*/
