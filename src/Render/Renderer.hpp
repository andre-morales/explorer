#include "Renderer.h"
#include <cmath>
#define RADS (3.14159265359/180.0)

static constexpr float skybox_verts[] = {
	// Left
	-.5, .5, -.5,
	-.5, .5, .5,
	-.5, -.5, .5,
	-.5, -.5, -.5,

	// Front
	.5, .5, -.5,
	-.5, .5, -.5,
	-.5, -.5, -.5,
	.5, -.5, -.5,

	// Right
	.5, .5, .5,
	.5, .5, -.5,
	.5, -.5, -.5,
	.5, -.5, .5,

	// Back
	-.5, .5, .5,
	.5, .5, .5,
	.5, -.5, .5,
	-.5, -.5, .5,

	// Bottom
	.5, -.5, -.5,
	-.5, -.5, -.5,
	-.5, -.5, .5,
	.5, -.5, .5,

	// Top
	.5, .5, -.5,
	-.5, .5, -.5,
	-.5, .5, .5,
	.5, .5, .5,
};
static constexpr float skybox_uvs[] = {
	// Left
	0.250, 0.3333333333,
	0.000, 0.3333333333,
	0.000, 0.6666666666,
	0.250, 0.6666666666,

	// Front
	0.500, 0.3333333333,
	0.250, 0.3333333333,
	0.250, 0.6666666666,
	0.500, 0.6666666666,

	// Right
	0.750, 0.3333333333,
	0.500, 0.3333333333,
	0.500, 0.6666666666,
	0.750, 0.6666666666,

	// Back
	1.000, 0.3333333333,
	0.750, 0.3333333333,
	0.750, 0.6666666666,
	1.000, 0.6666666666,

	// Bottom
	0.500, 0.66666666666666,
	0.250, 0.66666666666666,
	0.250, 1.0000,
	0.500, 1.0000,

	// Top
	0.500, 0.3333333333,
	0.250, 0.3333333333,
	0.250, 0.0000,
	0.500, 0.0000,
};
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
static float cosdf(float angle){ return std::cosf(angle*RADS); }
static float sindf(float angle){ return std::sinf(angle * RADS); }
/*static constexpr const char* __FACES[] = {"top", "bottom", "west", "east", "front", "back"};
static constexpr const char* __FACING[] = {	"north", "east", "south", "west" };
static constexpr const char* getFaceName(unsigned char face){ return __FACES[face]; }
static constexpr const char* getFacingDirection(double rx){ return __FACING[mod(floor(rx/90+0.5), 4)]; }*/
