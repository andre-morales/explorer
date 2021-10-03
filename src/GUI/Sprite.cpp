#include "Sprite.h"

Sprite::Sprite(const Shared<Texture>& tex) : Sprite(tex, ScalingMode::STRETCH, {0, 0, 0, 0}){}
Sprite::Sprite(const Shared<Texture>& tex, ScalingMode sm, const vec4f& b)
: texture(tex), scalingMode(sm), borders(b){}