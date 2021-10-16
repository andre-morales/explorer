#pragma once
#include "ilib/Math/vec4.h"
#include "Game/Planet.h"
#include <iostream>
#define TO_RADS (3.14159265359/180.0)

static float cosdf(float angle){ return std::cosf(angle * TO_RADS); }
static float sindf(float angle){ return std::sinf(angle * TO_RADS); }

class Renderer;

class RendererImpl {
public:
	Renderer& This;
	vec4f sunDiffuse, sunAmbient;
	vec4f sunVector;

	RendererImpl(class Renderer& r) : This(r){}
	void makeSun(Planet&);
	void drawGame11(class GLContext&);
	void drawGame15(GLContext&);
	void drawGame20(GLContext&);
	void uploadChunkVBO(class Chunk*);
	void renderUI(GLContext&);
	void bindTexture(const char*);
};

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
	.5, .5,  .5,
	-.5, .5,  .5,
	-.5, .5, -.5,
	.5, .5, -.5, 
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
