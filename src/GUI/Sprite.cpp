#include "Sprite.h"

Sprite::Sprite(){}
Sprite::Sprite(Shared<Texture> tex){
    setTexture(tex);
}
Sprite::~Sprite(){}

 void Sprite::setTexture(Shared<Texture> tex){
     this->texture = tex;
 }
