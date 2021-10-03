#pragma once
#include "ilib/Math/vec4.h"
#include "ilib/mem.h"
class Texture;

enum class ScalingMode {
	STRETCH, SPRITE
};

class Sprite {
public:
	Sprite(const Shared<Texture>&);
	Sprite(const Shared<Texture>&, ScalingMode, const vec4f&);

	Shared<Texture> texture;
	ScalingMode scalingMode;
	vec4f borders;
};
