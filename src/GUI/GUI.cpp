#include "GUI.h"
#include "Explorer/Window.h"
#include "Render/GLContext.h"
#include "GUI/Components/Component.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Event/MouseButtonEvent.h"
#include "GUI/Event/MouseMotionEvent.h"
#include "GUI/Event/KeyEvent.h"
#include "GUI/Event/CharEvent.h"
#include "ilib/Math/mat4.h"
#include "GL/glew.h"
#include <iostream>

GUI::GUI(Shared<GLContext> glc) : context(glc), root(new Component(new StackLayout())){
	root->setBackground(false);
	root->ownerGUI = this;
	auto win = glc->window.lock();
	setBounds(0, 0, win->getWidth(), win->getHeight());
	registerListeners();
}

GUI::~GUI(){
	unregisterListeners();
}

void GUI::registerListeners(){
	auto glc = context.lock();
	auto window = glc->window.lock();
	Window& win = *window.get();

	frameSizeListener = win.addFrameSizeListener([this](int w, int h){
		setBounds(0, 0, w, h);
	});
	mouseButtonListener = win.addMouseButtonListener([this, win](int btn, int act, int mods){
		double mx, my;
		win.getMousePosition(&mx, &my);

		if(root->visible) onMouseButton(btn, act, mods, mx, my);
	});
	mouseMotionListener = win.addMouseMotionListener([this](double x, double y){
		if(root->visible) onMouseMotion(x, y);
	});
	keyListener = win.addKeyListener([this](int key, int scan, int act, int mods){
		if(root->visible) onKey(key, scan, act, mods);
	});
	charListener = win.addCharListener([this](uint32 chr){
		if(root->visible) onChar(chr);
	});
}

void GUI::unregisterListeners(){
	auto win = context.lock()->window.lock();
	if(frameSizeListener) win->removeFrameSizeListener(frameSizeListener);
	if(mouseButtonListener) win->removeMouseButtonListener(mouseButtonListener);
	if(mouseMotionListener) win->removeMouseMotionListener(mouseMotionListener);
	if(keyListener) win->removeKeyListener(keyListener);
	if(charListener) win->removeCharListener(charListener);
}

void GUI::giveFocus(sh<Component> fc){
	auto ofc = focusedComponent.lock();
	if(fc != ofc){
		if(fc && fc->focusable){
			fc->focused = true;
		}

		focusedComponent = fc;
		if(ofc) ofc->focused = false;
	}
}

void GUI::releaseFocus(Component* fc){
	if(!fc || fc == focusedComponent.lock().get()){
		if(fc) fc->focused = false;
		focusedComponent.reset();
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
	for (auto it = pan->children.rbegin(); it != pan->children.rend(); ++it) {
		auto& c = **it;
		if(c.visible && c.enabled){

			float cx = anch.x + c.x;
			float cy = anch.y + c.y;
			float cw = c.width;
			float ch = c.height;

			if(sc.x >= cx && sc.x <= cx + cw && sc.y >= cy && sc.y <= cy + ch){
				if(c.children.size() > 0){
					propagateOnMouseButton(*it, {cx, cy, cw, ch}, ev, sc);
				} else {
					float cmpx = sc.x - cx;
					float cmpy = sc.y - cy;
					fireOnMouseButton(*it, ev, sc, {cmpx, cmpy}, true);
				}
				return;
			}
		}
	}
	fireOnMouseButton(pan, ev, sc, {panx, pany}, false);
}

void GUI::fireOnMouseButton(Shared<Component> fc, const vec3<byte>& ev, const vec2f& sc, const vec2f& cc, bool fs){
	if(fs) fc->fireMouseButtonListeners({fc.get(), ev.x, ev.y, ev.z, sc, cc}, true);

	giveFocus(fc);

	fc->fireMouseButtonListeners({fc.get(), ev.x, ev.y, ev.z, sc, cc});
}

void GUI::onMouseMotion(float mx, float my){
	Component& r = *root;
	if(mx >= r.x && mx <= r.x + r.width && my >= r.y && my <= r.y + r.height){
		propagateOnMouseMotion(root, {r.x, r.y, r.width, r.height}, {mx, my});
	}
}

void GUI::propagateOnMouseMotion(Shared<Component> pan, const vec4f& anch, const vec2f& sc){
	float panx = sc.x - anch.x;
	float pany = sc.y - anch.y;
	pan->fireMouseMotionListeners({pan.get(), 0, sc, {panx, pany}}, true);
	for (auto it = pan->children.rbegin(); it != pan->children.rend(); ++it) {
		Component& c = **it;
		if(c.visible && c.enabled){
			float cx = anch.x + c.x;
			float cy = anch.y + c.y;
			float cw = c.width;
			float ch = c.height;
			if(sc.x >= cx && sc.x <= cx + cw && sc.y >= cy && sc.y <= cy + ch){
				if(c.children.size() > 0){
					propagateOnMouseMotion(*it, {cx, cy, cw, ch}, sc);
				} else {
					float cmpx = sc.x - cx;
					float cmpy = sc.y - cy;
					fireOnMouseMotion(*it, sc, {cmpx, cmpy}, true);
				}
				// Click went in some component inside me, don't fire my own listeners.
				return;
			}
		}
	}
	fireOnMouseMotion(pan, sc, {panx, pany}, false);
}

void GUI::fireOnMouseMotion(Shared<Component> hc, const vec2f& sc, const vec2f& cc, bool fs){
	if(fs) hc->fireMouseMotionListeners({hc.get(), 0, sc, cc}, true);

	auto ohc = hoveredComponent.lock(); // Old focused component.
	if(ohc){
		if(ohc.get() != hc.get()){
			// Tell old component that is not being hovered anymore.
			ohc->fireMouseMotionListeners({ohc.get(), 2, sc, cc});

			// Tell new component that it got focused.
			hc->fireMouseMotionListeners({hc.get(), 1, sc, cc});
			hoveredComponent = hc;
		} else {

			// Just hovering.
			hc->fireMouseMotionListeners({hc.get(), 0, sc, cc});
		}
	} else {
		// Tell new component that it got focused.
		hoveredComponent = hc;
		hc->fireMouseMotionListeners({hc.get(), 1, sc, cc});
	}
}

void GUI::onKey(int key, int scan, int act, int mods){
	auto fc = focusedComponent.lock();
	if(!fc) return;

	auto ev = KeyEvent(fc.get(), key, scan, act, mods);
	fc->fireKeyListeners(ev);

	Component* c = fc->parent;
	while(c != nullptr){
		c->fireKeyListeners(ev, true);
		c = c->parent;
	}
}

void GUI::onChar(uint32 chr){
	auto fc = focusedComponent.lock();
	if(!fc) return;

	auto ev = CharEvent(fc.get(), chr);
	fc->fireCharListeners(ev);
}

void GUI::render(){
	if(!root->visible) return;

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

void GUI::add(Shared<Component> c, int p){
	root->add(c, p);
}

Shared<Component> GUI::remove(const std::string& name){
	return root->remove(name);
}

void GUI::setVisible(bool v){
	root->visible = v;
}
bool GUI::isVisible(){
	return root->visible;
}

void GUI::setBackground(bool v){
	root->setBackground(v);
}

void GUI::setBounds(float x, float y, float w, float h){
	root->setBounds(x, y, w, h);
}

vec4f GUI::getBounds(){
	return root->getBounds();
}
