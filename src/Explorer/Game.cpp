#include "Game.h"
#include "Explorer/Explorer.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/BufferedSocket.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>
#define RADS (3.14159265359/180.0)

Game::Game(Explorer& exp)
: explorer(exp), camera(new Camera()){
	lastMX = -1;
	lastMY = -1;
}

Game::~Game(){
	Window& win = *explorer.renderer->window;
	win.removeMouseButtonListener((Window::MouseButtonListener*)mouseButtonListener);
	win.removeMouseMotionListener((Window::MouseMotionListener*)mouseMotionListener);
	win.removeKeyListener((Window::KeyListener*)keyListener);
	win.removeCharListener((Window::CharListener*)charListener);
}

void Game::initInput(){
	Window& win = *explorer.renderer->window;
	mouseButtonListener = win.addMouseButtonListener([this](int btn, int act, int mods){

	});
	mouseMotionListener = win.addMouseMotionListener([this](double mx, double my){
		if(lastMX != -1 && !chatOpen){
			auto& rot = camera->rot;
			rot.x += my - lastMY;
			rot.y += mx - lastMX;

			if(rot.x < -90) rot.x = -90;
			if(rot.x > 90) rot.x = 90;
		}
		lastMX = mx;
		lastMY = my;
	});
	keyListener = win.addKeyListener([this](int key, int scan, int act, int mods){
		if(chatOpen){
			if(key == GLFW_KEY_ESCAPE && act == 1){
				chatOpen = false;
				chatOpening = false;
				chatText = "";
				explorer.renderer->window->holdCursor();
			}
			if(key == GLFW_KEY_BACKSPACE && act == 1){
				if(chatText.length() > 0){
					chatText.pop_back();
				}
			}
			if(key == GLFW_KEY_ENTER && act == 1){
				sendChatMessage();
				chatText = "";
			}
		} else {
			if(key == GLFW_KEY_T && act == 1){
				chatOpen = true;
				chatOpening = true;
				explorer.renderer->window->freeCursor();
			}
		}
	});
	charListener = win.addCharListener([this](unsigned int chr){
		if(chatOpen && !chatOpening){
			if(chr < 255){
				chatText += (unsigned char)chr;
			}
		}
		chatOpening = false;
	});
}

void Game::update(){
	auto& cm = *camera;
	auto& win = *explorer.renderer->window;
	float speed = 0.02;
	if(!chatOpen){
		if(win.getKey(GLFW_KEY_A)){
			cm.pos.x -= cos(cm.rot.y * RADS) * speed;
			cm.pos.z -= sin(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(GLFW_KEY_D)){
			cm.pos.x += cos(cm.rot.y * RADS) * speed;
			cm.pos.z += sin(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(GLFW_KEY_W)){
			cm.pos.x += sin(cm.rot.y * RADS) * speed;
			cm.pos.z -= cos(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(GLFW_KEY_S)){
			cm.pos.x -= sin(cm.rot.y * RADS) * speed;
			cm.pos.z += cos(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(GLFW_KEY_SPACE)){
			cm.pos.y += speed;
		}
		if(win.getKey(GLFW_KEY_LEFT_SHIFT)){
			cm.pos.y -= speed;
		}
	}
}

void Game::sendChatMessage(){
	Packet packet;
	packet.opcode = (unsigned int) PacketCodes::CHAT_MSG;
	packet.data = (byte*)chatText.c_str();
	packet.length = chatText.length();
	packet.send(*((Stream*)socket->sock));
}

void Game::join(){
	Packet packet;
	packet.opcode = (unsigned int) PacketCodes::JOIN;
	packet.data = (byte*)"Hello world!";
	packet.length = 13;
	packet.send(*((Stream*)socket->sock));
}
