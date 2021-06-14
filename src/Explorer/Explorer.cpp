#include "Explorer.h"
#include "Explorer/Game.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Component.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/StackPane.h"
#include "GUI/Components/SplitPane.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "Render/GL.h"
#include "Render/Renderer.h"
#include "Render/Image.h"
#include "Render/Texture.h"
#include "Render/TextureAtlas.h"
#include "ilib/Net/BufferedSocket.h"
#include "ilib/Net/Socket.h"
#include "Exception.h"
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include <iostream>
#include <thread>

Explorer* explorer;

int main(){
	std::cout << "Starting up...\n";
	try {
		explorer = new Explorer();
		explorer->init();
		explorer->run();
		explorer->shutdown();
		return 0;
	} catch(const Exception& ex){
		std::cerr << "Exception '" << ex.message << "'.\n";
	} catch(const std::exception& ex){
		std::cerr << "std::exception '" << ex.what() << "'.\n";
	} catch(...){
		std::cerr << "Unknown error.\n";
		return -1;
	}
}

Explorer::Explorer(){
	renderer = mkUnique<Renderer>(*this);
}

Explorer::~Explorer(){

}

void Explorer::init(){
	renderer->init(GL::V1_1);
    renderer->begin();
    attachedGUIs.push_back(createStartGUI());
    explorerInstance = mkUnique<Instance>();
    explorerInstance->explorer = this;
    handleEventsForGUIs();
}

void Explorer::run(){
	Net::init();
	while(!renderer->window->getCloseFlag()){
		renderer->render();
		int err = glGetError();
		if(err != 0) std::cerr << "GL Error: " << err << "\n";

		renderer->window->bufferSwap();
		update();
	}
}

void Explorer::update(){
    Window::fireEvents();

    if(gameInstance){
        gameInstance->update();
    }

}

Shared<GUI> Explorer::createMainMenuGUI(){
	auto gui = mkShared<GUI>(renderer->context);
	gui->root->setBackground(renderer->textures["title_bg"]);
	gui->root->bgSprite->scalingMode = ScalingMode::STRETCH;
	auto root = mkShared<SplitPane>();
	gui->root->add(root);
	root->setBackground({0, 0, 0, 0});
	root->value = 0.35;
	root->insets = {8, 8, 8, 8};

	auto menu = mkShared<GridPane>();
	root->add(menu);
	menu->orientation = (GridPane::LR_BT);
	menu->setGrid(1, 8);
	menu->setSpacing(8, 4);
	menu->insets = {8, 8, 8, 8};
	menu->setBackground({1, 1, 1, 0.5f});
	menu->setBackground(renderer->textures["gui_panel2"]);
	menu->bgSprite->scalingMode = ScalingMode::SPRITE;
	menu->spriteBorders = {.0625, .0625, .0625, .0625};

	auto exitBtn = mkShared<Button>("Exit");
	exitBtn->setBackground(renderer->textures["gui_button"]);
	exitBtn->bgSprite->scalingMode = ScalingMode::SPRITE;
	exitBtn->setFont(renderer->font);
	exitBtn->addActionListener([this](auto&){
		renderer->window->setCloseFlag(true);
	});
	exitBtn->spriteBorders = {.0625, .0625, .0625, .0625};
	menu->add(exitBtn);

	Shared<TextField> field = mkShared<TextField>("127.0.0.1");

	auto playBtn = mkShared<Button>("Join game...");
	playBtn->setBackground(renderer->textures["gui_button"]);
	playBtn->bgSprite->scalingMode = ScalingMode::SPRITE;
	playBtn->setFont(renderer->font);
	playBtn->addActionListener([this, field](const ActionEvent&){
		try {
            auto* sock = new BufferedSocket(new Socket(field->text.c_str(), 3333));
            sock->init(4096);
            gameInstance = mkUnique<Game>(*this);
            gameInstance->initInput();
            gameInstance->socket = sock;
            gameInstance->join();

            attachedGUIs.clear();

            auto universe = mkShared<Universe>(explorerInstance.get());
            auto planet = mkShared<Planet>(universe.get());

            auto ch = new Chunk(planet.get(), 0, 0, 0);
            ch->gen();
            ch->batch();
            planet->chunkSet.insert(ch);

            universe->planets.emplace_back(planet);
            explorerInstance->universes.emplace_back(universe);
            gameInstance->universe = universe;

            renderer->window->holdCursor();
		}catch(...){}
	});

	playBtn->spriteBorders = {.0625, .0625, .0625, .0625};
	menu->add(playBtn);


	field->setBackground(renderer->textures["gui_panel2"]);
	field->setFont(renderer->font);
	field->bgSprite->scalingMode = ScalingMode::SPRITE;
	field->spriteBorders = {.0625, .0625, .0625, .0625};
	menu->add(field);
	return gui;
}

Shared<GUI> Explorer::createStartGUI(){
	auto gui = mkShared<GUI>(renderer->context);

	auto root = mkShared<SplitPane>();
	gui->root->add(root);
	root->setBackground(mkShared<Texture>("res\\title_bg.png"));
	root->bgSprite->scalingMode = ScalingMode::STRETCH;
	root->setBackground({1, 1, 1, 1});
	root->value = 0.35;
	root->insets = {8, 8, 8, 8};

	auto menu = mkShared<GridPane>();
	root->add(menu);

	menu->orientation = (GridPane::LR_BT);
	menu->setGrid(1, 16);
	menu->setSpacing(8, 4);
	menu->insets = {8, 8, 8, 8};
	menu->setBackground({1, 1, 1, 0.5f});
	menu->setBackground(renderer->textures["gui_panel2"]);
	menu->bgSprite->scalingMode = ScalingMode::SPRITE;
	menu->spriteBorders = {.0625, .0625, .0625, .0625};

	auto beginBtn = gui_button("Begin!");
	menu->add(beginBtn);
	beginBtn->addActionListener([this](auto&){
		attachedGUIs.clear();
		renderer->shutdown();

		screen = Screen::START_SCREEN;
		renderer->init(GL::V1_1);
		attachedGUIs.emplace_back(createMainMenuGUI());
	});
	return gui;
}

Shared<Button> Explorer::gui_button(const char* text){
	auto btn = mkShared<Button>(text);
	btn->setFont(renderer->font);
	btn->setBackground(renderer->textures["gui_button"]);
	btn->bgSprite->scalingMode = ScalingMode::SPRITE;
	btn->spriteBorders = {.0625, .0625, .0625, .0625};
	return btn;
}

void Explorer::handleEventsForGUIs(){
	renderer->window->addMouseButtonListener([this](int btn, int act, int mods){
		for (auto& gui : attachedGUIs) {
			if(gui->root->visible){
				double mx, my;
				renderer->window->getMousePosition(&mx, &my);

				gui->onMouseButton(btn, act, mods, mx, my);
			}
		}
	});
	renderer->window->addMouseMotionListener([this](double x, double y){
		for (auto& gui : attachedGUIs) {
			if(gui->root->visible){
				gui->onMouseMotion(x, y);
			}
		}
	});
	renderer->window->addKeyListener([this](int key, int scan, int act, int mods){
		for (auto& gui : attachedGUIs) {
			if(gui->root->visible){
				gui->onKey(key, scan, act, mods);
			}
		}
	});
}

void Explorer::shutdown(){

}
