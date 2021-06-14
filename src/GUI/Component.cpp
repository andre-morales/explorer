#include "Component.h"
#include "GUI/Sprite.h"
#include "Render/GLContext.h"
#include "Render/Texture.h"
#include "Render/Geometry.h"
#include "Exception.h"
#include "GL/glew.h"
#include <iostream>

Component::Component(){
	this->parent = nullptr;
	this->focusable = false;
	this->visible = true;
	this->enabled = true;
	this->insets = {0, 0, 0, 0};
	this->background = true;
	this->bgColor = {1, 1, 1, 1};
	this->fgColor = {1, 1, 1, 1};
	this->spriteBorders = {0, 0, 0, 0};
}
Component::~Component(){}

Shared<Component> Component::add(Shared<Component> c){
	children.emplace_back(c);
	return c;
}

void Component::paint(GLContext& c, vec2f anch){
	paintComponent(c, anch);
	paintChildren(c, anch);
}
void Component::paintComponent(GLContext& gi, vec2f anch){
	gi.enableVertsArray();

	if(background){
		float cwidth = gi.getWidth();
		float cheight = gi.getHeight();
		float aspect = cwidth / cheight;

		const float& ax = anch.x, ay = anch.y, aw = width, ah = height;

		const float a = bgColor.a;
		glColor4f(bgColor.r, bgColor.g, bgColor.b, bgColor.a);
		if(a < 0.999){
			gi.disableAlphaTesting();
			gi.setBlending(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			gi.enableBlending();
		} else {
			gi.disableBlending();
		}

		if(!bgSprite){
			gi.disableTexture2d();
			const float verts[] = {
				ax, ay,
				ax+aw, ay,
				ax+aw, ay+ah,
				ax, ay+ah
			};

			glVertexPointer(2, GL_FLOAT, 0, verts);
			glDrawArrays(GL_QUADS, 0, 4);
		} else {
		    auto& texture = *bgSprite->texture;

			gi.enableTexture2d();
			gi.enableUVsArray();
			texture.bind();
			const float cverts[] = {
				ax, ay,
				ax+aw, ay,
				ax+aw, ay+ah,
				ax, ay+ah
			};
			switch(bgSprite->scalingMode) {
			default:
			case ScalingMode::STRETCH:
				glTexCoordPointer(2, GL_FLOAT, 0, Quad::uvs);
				glVertexPointer(2, GL_FLOAT, 0, cverts);
				glDrawArrays(GL_QUADS, 0, 4);
				break;
			case ScalingMode::SPRITE: {
				const vec4f& sb = spriteBorders;
				float lsw = sb.left * texture.width ;
				float rsw = sb.right * texture.width;
				float tsh = sb.top * texture.height ;
				float bsh = sb.bottom * texture.height;

				const float verts[] = {
					// Left Top
					ax,     ay,
					ax+lsw, ay,
					ax+lsw, ay+tsh,
					ax,     ay+tsh,

					// Left Middle
					ax,     ay+tsh,
					ax+lsw, ay+tsh,
					ax+lsw, ay+ah-bsh,
					ax,     ay+ah-bsh,

					// Left Bottom
					ax,     ay+ah-bsh,
					ax+lsw, ay+ah-bsh,
					ax+lsw, ay+ah,
					ax,     ay+ah,

					// Center Top
					ax+lsw,    ay,
					ax+aw-rsw, ay,
					ax+aw-rsw, ay+tsh,
					ax+lsw,    ay+tsh,

					// Center Middle
					ax+lsw,    ay+tsh,
					ax+aw-rsw, ay+tsh,
					ax+aw-rsw, ay+ah-bsh,
					ax+lsw,    ay+ah-bsh,

					// Center Bottom
					ax+lsw,    ay+ah-bsh,
					ax+aw-rsw, ay+ah-bsh,
					ax+aw-rsw, ay+ah,
					ax+lsw,    ay+ah,

					// Right Top
					ax+aw-rsw, ay,
					ax+aw,     ay,
					ax+aw,     ay+tsh,
					ax+aw-rsw, ay+tsh,

					// Right Middle
					ax+aw-rsw, ay+tsh,
					ax+aw,     ay+tsh,
					ax+aw,     ay+ah-bsh,
					ax+aw-rsw, ay+ah-bsh,

					// Right Bottom
					ax+aw-rsw, ay+ah-bsh,
					ax+aw,     ay+ah-bsh,
					ax+aw,     ay+ah,
					ax+aw-rsw, ay+ah,
				};
				const float uvs[] = {
					// Left Top
					0,       0,
					sb.left, 0,
					sb.left, sb.top,
					0,       sb.top,

					// Left Middle
					0,       sb.top,
					sb.left, sb.top,
					sb.left, 1-sb.bottom,
					0,       1-sb.bottom,

					// Left Bottom
					0,       1-sb.bottom,
					sb.left, 1-sb.bottom,
					sb.left, 1,
					0,       1,

					// Center Top
					sb.left, 0,
					1-sb.ri, 0,
					1-sb.ri, sb.top,
					sb.left, sb.top,

					// Center Middle
					sb.left, sb.top,
					1-sb.ri, sb.top,
					1-sb.ri, 1-sb.bottom,
					sb.left, 1-sb.bottom,

					// Center Bottom
					sb.left, 1-sb.bottom,
					1-sb.ri, 1-sb.bottom,
					1-sb.ri, 1,
					sb.left, 1,

					// Right Top
					1-sb.ri, 0,
					1,       0,
					1,       sb.top,
					1-sb.ri, sb.top,

					// Right Middle
					1-sb.ri, sb.top,
					1,       sb.top,
					1,       1-sb.bottom,
					1-sb.ri, 1-sb.bottom,

					// Right Bottom
					1-sb.ri, 1-sb.bottom,
					1,       1-sb.bottom,
					1,       1,
					1-sb.ri, 1,
				};

				glTexCoordPointer(2, GL_FLOAT, 0, uvs);
				glVertexPointer(2, GL_FLOAT, 0, verts);
				glDrawArrays(GL_QUADS, 0, 36);
				}
				break;
			}
		}
	}
}
void Component::paintChildren(GLContext& c, vec2f anch){
	for (auto& child : children) {
		child->paint(c, {anch.x + child->x, anch.y + child->y});
	}
}

void Component::setBackground(Color c){
	this->bgColor = c;
}

void Component::setBackground(Shared<Texture> tex){
	this->bgSprite = mkShared<Sprite>(tex);
}

void Component::setBackground(Shared<Sprite> sprite){
	this->bgSprite = sprite;
}

//void Component::setFont(Shared<Font> font){
//	this->font = font;
//}

void Component::setBounds(float x, float y, float w, float h){
	bool fire = false;
	if(this->width != w || this->height != h){
		fire = true;
	}
	this->x = x;
	this->y = y;
	this->width = w;
	this->height = h;
	if(fire){
		fireSizeListeners(w, h);
	}
}

vec4f Component::getBounds(){
    return {x, y, width, height};
}

vec2f Component::getPosition(){
	return {x, y};
}

// Super Listeners get called even when an event is still propagating trough its children.
// These are always called before the normal events are fired.
Component::MouseButtonListener* Component::addMouseButtonListener(MouseButtonListener listener, bool super){
	MouseButtonListener* mbl = new MouseButtonListener(listener);
	if(super){
		superMouseButtonListeners.emplace_back(mbl);
	} else {
		mouseButtonListeners.emplace_back(mbl);
	}
	return mbl;
}
void Component::fireMouseButtonListeners(const MouseButtonEvent& ev, bool super) {
	if(super){
		for(auto& p : superMouseButtonListeners) (*p)(ev);
	} else {
		for(auto& p : mouseButtonListeners) (*p)(ev);
	}
}
void Component::removeMouseButtonListener(MouseButtonListener* mbl, bool super) {
	if(super){
		for(auto it = begin(superMouseButtonListeners); it != end(superMouseButtonListeners); ++it){
			if(*it == mbl){
				superMouseButtonListeners.erase(it);
				return;
			}
		}
		throw Exception("Trying to remove a non-existent MouseButtonListener.");
	} else {
		throw Exception("Not implemented yet.");
	}
}

// --
Component::MouseMotionListener* Component::addMouseMotionListener(MouseMotionListener listener, bool super){
	MouseMotionListener* mml = new MouseMotionListener(listener);
	if(super){
		superMouseMotionListeners.emplace_back(mml);
	} else {
		mouseMotionListeners.emplace_back(mml);
	}
	return mml;
}
void Component::fireMouseMotionListeners(const MouseMotionEvent& ev, bool super) {
	if(super){
		for(auto& p : superMouseMotionListeners) (*p)(ev);
	} else {
		for(auto& p : mouseMotionListeners) (*p)(ev);
	}
}
void Component::removeMouseMotionListener(MouseMotionListener* mml, bool super) {
	throw Exception("Not implemented yet.");
}

// --
Component::KeyListener* Component::addKeyListener(KeyListener listener){
	KeyListener* kl = new KeyListener(listener);
	keyListeners.emplace_back(kl);
	return kl;
}

void Component::fireKeyListeners(const KeyEvent& ev) {
	for(auto& p : keyListeners) (*p)(ev);
}
void Component::removeKeyListener(KeyListener* kl) {
	throw Exception("Not implemented yet.");
}

// --
Component::SizeListener* Component::addSizeListener(SizeListener listener){
	SizeListener* sl = new SizeListener(listener);
	sizeListeners.emplace_back(sl);
	return sl;
}

void Component::fireSizeListeners(float w, float h) {
	for(auto& p : sizeListeners) (*p)(w, h);
}
void Component::removeSizeListener(SizeListener* sl) {
	throw Exception("Not implemented yet.");
}

