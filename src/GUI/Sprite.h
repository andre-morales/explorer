#pragma once
#include "ilib/mem.h"
class Texture;

enum class ScalingMode {
	STRETCH, SPRITE,
};

class Sprite {
public:
    Sprite();
    Sprite(Shared<Texture>);
    ~Sprite();

    ScalingMode scalingMode;
    Shared<Texture> texture;

    void setTexture(Shared<Texture>);
private:
    Sprite(const Sprite&) = delete;
};
