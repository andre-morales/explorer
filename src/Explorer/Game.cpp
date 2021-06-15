#include "Game.h"
#include "Explorer/Explorer.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "GUI/GUI.h"
#include "GUI/Component.h"
#include "GUI/Sprite.h"
#include "GUI/Components/SplitPane.h"
#include "GUI/Components/GridPane.h"
#include "Render/Renderer.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/BufferedSocket.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <cmath>
#include <cstring>
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

void Game::init(){
	auto mg = makePauseMenuGUI();
	menuGUI = mg.get();
	explorer.attachedGUIs.emplace_back(mg);

	Window& win = *explorer.renderer->window;
	mouseButtonListener = win.addMouseButtonListener([this](int btn, int act, int mods){

	});
	mouseMotionListener = win.addMouseMotionListener([this](double mx, double my){
		if(lastMX != -1 && !uiOpen){
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
		if(key == GLFW_KEY_ESCAPE && act == 1){
			if(menuOpen){
				closeMenu();
			} else if(chatOpen){
				closeChat();
			} else {
				openMenu();
			}
		}

		if(chatOpen){
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
				openChat();
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


std::string tosdtr(byte* ptr, int len){
    std::string a("");
	for(int i = 0; i < len; i++){
		a += std::to_string(ptr[i]);
		if(i < len - 1){
			a += std::string(", ");
		}

	}
	return a;
}
void printPacket(const Packet& packet){
    std::cout << "Packet(" << packet.opcode << ")[" << packet.length << "] = {" << tosdtr(packet.data, packet.length) << "}\n";
}

void Game::update(){
    auto* packet = readNetPacket();
    if(packet){
        processNetPacket(*packet);
        delete packet;
    }

	auto& cm = *camera;
	auto& ei = *explorer.explorerInstance;
	auto& pl = *ei.universes[0]->planets[0];
	auto& win = *explorer.renderer->window;
	float speed = 0.05;
	for(int cx = -1; cx <= 1; cx++){
		for(int cz = -1; cz <= 1; cz++){
			Chunk* ch = pl.getChunk(cx, 0, cz);
			if(!ch){
				auto* chunk = pl.allocateChunk(cx, 0, cz);
				chunk->gen();
				chunk->batch();

			}
		}
	}

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

void Game::openMenu(){
	uiOpen = true;
	menuOpen = true;
	explorer.renderer->window->freeCursor();
	menuGUI->root->visible = true;
}

void Game::closeMenu(){
	uiOpen = false;
	menuOpen = false;
	explorer.renderer->window->holdCursor();
	menuGUI->root->visible = false;
}

void Game::openChat(){
	uiOpen = true;
	chatOpen = true;
	chatOpening = true;
	explorer.renderer->window->freeCursor();
}

void Game::closeChat(){
	uiOpen = false;
	chatOpen = false;
	chatOpening = false;
	chatText = "";
	explorer.renderer->window->holdCursor();
}

Packet* Game::readNetPacket(){
    socket->clearup();
	socket->read(nullptr, 0);
	uint32 available = socket->available();
    if(available >= 6){
        byte* readHead = socket->buffer + socket->readPtr;

        uint32 len = *((uint32*)(readHead + 2));
        if(available >= len + 6){
            Packet* packet = new Packet();

            socket->read((byte*)&packet->opcode, 2);
            socket->read((byte*)&packet->length, 4);
            packet->data = new byte[len];
            socket->read(packet->data, len);

            return packet;
        }
    }
    return nullptr;
}

void Game::processNetPacket(Packet& packet){
    switch((PacketCode)packet.opcode){
    case PacketCode::CHAT_MSG: {
        std::string msg((char*)packet.data, packet.length);
        chatHistory.emplace_front(msg);
        std::cout << "Chat: " << msg << "\n";
        printPacket(packet);
        }
        break;
    default:
        printPacket(packet);
    }


    delete packet.data;
    packet.data = nullptr;
}

Shared<GUI> Game::makePauseMenuGUI(){
	auto gui = mkShared<GUI>(explorer.renderer->context);
	gui->root->visible = false;
	gui->root->setBackground({0, 0, 0, 0});

	auto root = mkShared<SplitPane>();
	gui->root->add(root);
	root->setBackground({0, 0, 0, 0});
	root->value = 0.7;

	auto leftBG = mkShared<Component>();
	root->add(leftBG);
	leftBG->setBackground({0.4, 0.6, 1, 0.4});

	auto sideMenu = mkShared<GridPane>();
	sideMenu->setBackground(explorer.renderer->textures["gui_panel"]);
	root->add(sideMenu);

	return gui;
}

void Game::sendChatMessage(){
	Packet packet;
	packet.opcode = (unsigned int) PacketCode::CHAT_MSG;
	packet.data = (byte*)chatText.c_str();
	packet.length = chatText.length();
	packet.send(socket->sock);
}

void Game::join(){
	Packet packet;
	packet.opcode = (unsigned int) PacketCode::JOIN;
	packet.data = (byte*)name.c_str();
	packet.length = strlen((char*)packet.data);
	packet.send(socket->sock);
}

