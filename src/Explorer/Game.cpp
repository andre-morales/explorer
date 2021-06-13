#include "Game.h"
#include "Explorer/Explorer.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Render/Renderer.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/BufferedSocket.h"
#include <iostream>
#include <cmath>

Game::Game(Explorer& exp)
: explorer(exp),camera(new Camera()){
    lastMX = -1;
    lastMY = -1;
}

Game::~Game(){
    Window& win = *explorer.renderer->window;
    win.removeMouseMotionListener((Window::MouseMotionListener*)mouseMotionListener);
}

void Game::initInput(){
    Window& win = *explorer.renderer->window;
    mouseMotionListener = win.addMouseMotionListener([this](double mx, double my){
        if(lastMX != -1){
            auto& rot = camera->rot;
            rot.x += my - lastMY;
            rot.y += mx - lastMX;

            if(rot.x < -90) rot.x = -90;
            if(rot.x > 90) rot.x = 90;
        }
        lastMX = mx;
        lastMY = my;
    });
}

void Game::join(){
	Packet packet;
	packet.opcode = (unsigned int) PacketCodes::JOIN;
	packet.data = (byte*)"Hello world!";
	packet.length = 13;
	packet.send(*((Stream*)socket->sock));
}
