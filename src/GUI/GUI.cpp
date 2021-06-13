#include "GUI.h"
#include "GUI/Component.h"
#include "GUI/Components/StackPane.h"
#include "GL/glew.h"
#include "Render/GLContext.h"
#include "Explorer/Window.h"
#include "GUI/Event/MouseButtonEvent.h"
#include "GUI/Event/MouseMotionEvent.h"
#include "GUI/Event/KeyEvent.h"
#include "Math/mat4.h"
#include <iostream>

GUI::GUI(Shared<GLContext> glc) : context(glc){
	hoveredComponent = nullptr;
	root = mkShared<StackPane>();
	auto win = glc->window.lock();
	frameSizeListener = win->addFrameSizeListener([this](int w, int h){
		root->setBounds(0, 0, w, h);
	});
	root->setBounds(0, 0, win->getWidth(), win->getHeight());
}

GUI::~GUI(){
	auto glc = context.lock();
	auto win = glc->window.lock();
	win->removeFrameSizeListener((Window::FrameSizeListener*)frameSizeListener);
}

void GUI::onMouseMotion(float mx, float my){
    Component& r = *root;
	if(mx >= r.x && mx <= r.x + r.width && my >= r.y && my <= r.y + r.height){
		propagateOnMouseMotion(root.get(), {r.x, r.y, r.width, r.height}, {mx, my});
	}
}

void GUI::propagateOnMouseMotion(Component* pan, const vec4f& anch, const vec2f& sc){
	float panx = sc.x - anch.x;
	float pany = sc.y - anch.y;
	pan->fireMouseMotionListeners({pan, 0, sc, {panx, pany}}, true);
	for(auto& child : pan->children){
		Component& c = *child;
		if(c.visible && c.enabled){
			float cx = anch.x + c.x;
			float cy = anch.y + c.y;
			float cw = c.width;
			float ch = c.height;
			if(sc.x >= cx && sc.x <= cx + cw && sc.y >= cy && sc.y <= cy + ch){
				if(c.children.size() > 0){
					propagateOnMouseMotion(child.get(), {cx, cy, cw, ch}, sc);
				} else {
					float cmpx = sc.x - cx;
					float cmpy = sc.y - cy;
					fireOnMouseMotion(child.get(), sc, {cmpx, cmpy}, true);
				}
				// Click went in some component inside me, don't fire my own listeners.
				return;
			}
		}
	}
	fireOnMouseMotion(pan, sc, {panx, pany}, false);
}

void GUI::fireOnMouseMotion(Component* cmp, const vec2f& sc, const vec2f& cc, bool fs){
	if(fs) cmp->fireMouseMotionListeners({cmp, 0, sc, cc}, true);
	if(hoveredComponent != cmp){
		// Entering a new Component. Tell previous component we're not above it anymore.
		if(hoveredComponent != nullptr){
			hoveredComponent->fireMouseMotionListeners({hoveredComponent, 2, sc, cc});
		}
		cmp->fireMouseMotionListeners({cmp, 1, sc, cc});
		hoveredComponent = cmp;
	} else {
		// Just moving above the same component.
		cmp->fireMouseMotionListeners({cmp, 0, sc, cc});
	}
}

void GUI::onMouseButton(byte btn, byte act, byte mods, float mx, float my){
    Component& r = *root;

	if(mx >= r.x && mx <= r.x + r.width && my >= r.y && my <= r.y + r.height){
		propagateOnMouseButton(root, {r.x, r.y, r.width, r.height}, {btn, act, mods}, {mx, my});
	}
}

void GUI::propagateOnMouseButton(Shared<Component> pan, const vec4f& anch, const vec3<byte>& ev, const vec2f& sc){
	float panx = sc.x - anch.x;
	float pany = sc.y - anch.y;
	pan->fireMouseButtonListeners({pan.get(), ev.x, ev.y, ev.z, sc, {panx, pany}}, true);
	for(auto& child : pan->children){
		Component& c = *child;
		if(c.visible && c.enabled){

			float cx = anch.x + c.x;
			float cy = anch.y + c.y;
			float cw = c.width;
			float ch = c.height;

			if(sc.x >= cx && sc.x <= cx + cw && sc.y >= cy && sc.y <= cy + ch){

				if(c.children.size() > 0){
					propagateOnMouseButton(child, {cx, cy, cw, ch}, ev, sc);
				} else {
					float cmpx = sc.x - cx;
					float cmpy = sc.y - cy;
					fireOnMouseButton(child, ev, sc, {cmpx, cmpy}, true);
				}
				return;
			}
		}
	}
	fireOnMouseButton(pan, ev, sc, {panx, pany}, false);
}

void GUI::fireOnMouseButton(Shared<Component> fc, const vec3<byte>& ev, const vec2f& sc, const vec2f& cc, bool fs){
	if(fs) fc->fireMouseButtonListeners({fc.get(), ev.x, ev.y, ev.z, sc, cc}, true);

	auto ofc = focusedComponent.lock(); // Old focused component.
	if(ofc){
		if(fc.get() != ofc.get()){
			ofc->focused = false;

			if(fc->focusable){
				focusedComponent = fc;
				fc->focused = true;
			}
		}
	} else {
		if(fc->focusable){
			focusedComponent = fc;
			fc->focused = true;
		}
	}


	fc->fireMouseButtonListeners({fc.get(), ev.x, ev.y, ev.z, sc, cc});
}

void GUI::onKey(int key, int scan, int act, int mods){
	auto fc = focusedComponent.lock();
	if(!fc) return;

	fc->fireKeyListeners({fc.get(), key, scan, act, mods});
}


void GUI::render(){
	glMatrixMode(GL_PROJECTION);
	auto glc = context.lock();
	float hw = glc->getWidth() / 2.0f;
	float hh = glc->getHeight() / 2.0f;
	glLoadMatrixf(mat4<float>::ortho(-hw, hw, hh, -hh, -1, 1));
	glTranslatef(-hw, -hh, 0);


	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	root->paint(*glc, root->getPosition());
}
