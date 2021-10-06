#include "Explorer.h"
#include "ExplorerImpl.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Game.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Explorer/ConsoleUI.h"
#include "Explorer/StartUI.h"
#include "Explorer/MainMenuUI.h"
#include "Game/Instance.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "Game/Event.h"
#include "Game/TextLog.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "GUI/Keys.h"
#include "Render/GL.h"
#include "Render/Renderer.h"
#include "ilib/Net/BufferedSocket.h"
#include "ilib/Net/Socket.h"
#include "ilib/itime.h"
#include "Exception.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <assert.h>
#include "cwarns.h"

int main(){
	std::cout << "[-] Starting up...\n";
	try{
		{
			Explorer explorer{};
			explorer.init();
			explorer.run();
			explorer.shutdown();
		}
		std::cout << "[-] Bye!\n";
		return 0;
	} catch(const Exception& ex){
		std::cerr << "Exception '" << ex.message << "'.\n";
	} catch(const std::exception& ex){
		std::cerr << "std::exception '" << ex.what() << "'.\n";
	} catch(...){
		std::cerr << "Unknown error.\n";
	}

	std::cin.ignore();
	return 1;
}

Explorer::Explorer() : impl_(new ExplorerImpl(*this)), impl(*impl_){
	consoleLog.reset(new TextLog());
	log("Explorer", "Building...");
	renderer.reset(new Renderer(*this));
	log("Explorer", "Buit.");
}

Explorer::~Explorer(){
	log("Explorer", "Destroying...");
}

void Explorer::init(){
	log("Explorer", "Initializing...");
	renderer->init(GL::V1_1);
	begin();

	renderer->window->addKeyListener([this](int key, int, int act, int){
		if(act){
			IGNORE_WARN_PUSH
			IGNORE_WARN_DANGLING_TEMP_PNTR
			if(key == IKEY_F1){
				auto& con = *(ConsoleUI*)ui->getPanel("console").get();

				if((con.visible = !con.visible)){
					con.inputField->requestFocus();
				} else{
					con.inputField->releaseFocus();
				}
				if(gameInstance) gameInstance->gameFocused = !con.visible;
			} else if(key == IKEY_F2){
				auto& dbg = *ui->getPanel("debug_text").get();
				dbg.setVisible(!dbg.visible);
			}
			IGNORE_WARN_POP
		}
	});

	auto startUI = mkShared<StartUI>(*this);
	startUI->create();
	ui->addPanel(startUI, 0);

	explorerInstance = mkUnique<Instance>();
	log("Explorer", "Init Ok.");
}

void Explorer::begin(){
	renderer->loadResources();
	renderer->begin();
	ui.reset(new ExplorerGUI(*this));

	regCmd("window", [&w_ = renderer->window](auto args){
		if(args.size() < 2) return;
		auto& cmd = args[1];
		auto& win = *w_;
		if("full" == cmd){
			win.makeFullscreen();
		} else if("win" == cmd){
			win.makeWindowed();
		} else if("hold" == cmd){
			win.holdCursor();
		} else if("free" == cmd){
			win.freeCursor();
		}
	});

	ui->loadSprites();
	ui->addGUI(ui->createGUI(), "main", 0);

	auto conUI = mkShared<ConsoleUI>(*this);
	conUI->create();
	conUI->consoleLabel->text = consoleLog->asString();
	console = conUI;
	ui->addPanel(conUI);

	auto dbgText = ui->clLabel("");
	dbgText->enabled = false;
	dbgText->name = "debug_text";
	dbgText->textOrientation = Orientation::LEFT_TOP;
	dbgText->textSize = 2;
	dbgText->setBackground(false);
	ui->addPanel(dbgText);
}

void Explorer::registerBlocks(){
	log("Explorer", "Registering blocks...");
	auto& ei = *explorerInstance;
	ei.explorer = this;
	ei.registerBlock({"void",   0, 1.00, 0});
	ei.registerBlock({"air",    1, 0.00, 0});
	ei.registerBlock({"stone",  2, 1.00, 1});
	ei.registerBlock({"grass",  3, 1.00, 2, 3, 4});
	ei.registerBlock({"dirt",   4, 1.00, 4});
	ei.registerBlock({"glass",  5, 0.40f, 5});
	ei.registerBlock({"wood",   6, 1.00, 6});
	ei.registerBlock({"leaves", 7, 0.60f, 7});
	for(byte i = 8; i < 255; i++){
		ei.registerBlock({"unknown", i, 1, 63});
	}
}

void Explorer::run(){
	Net::init();

	auto now = timenow();
	auto ufTime = now; // Update frame time stamp
	auto ffTime = now; // FPS frame time stamp
	int frameCount = 0;

	while(!renderer->window->getCloseFlag()){
		now = timenow();
		double timeDelta = elapsedbetween(ufTime, now);
		ufTime = now;
		if(elapsedbetween(ffTime, now) >= 1){
			ffTime = now;
			fps = frameCount;
			frameCount = 0;
		}

		renderer->render();
		update(timeDelta);
		renderer->window->bufferSwap();

		++frameCount;
	}
}

void Explorer::update(double timeDelta){
	Window::fireEvents();

	if(gameInstance){
		gameInstance->update(timeDelta);
	}
	updateDebugText();

	auto& tsks = impl.tasks;
	for(auto it = tsks.begin(); it != tsks.end();){
		Task& task = *it;
		if(task.delay == 0){
			task.func();
			auto nit = tsks.erase(it);
			it = nit;
		} else{
			task.delay--;
			++it;
		}
	}
}

void Explorer::openMainMenu(){
	regTask(&Explorer::imp_openMainMenu, this);
}
void Explorer::joinGame(const std::string& addr, uint16 port, const std::string& name){
	regTask(&Explorer::imp_joinGame, this, addr, std::move(port), name);
}
void Explorer::localCommand(const std::string& cmd){
	regTask(&Explorer::imp_localCommand, this, cmd);
}
void Explorer::quitGame(){
	regTask(&Explorer::imp_quitGame, this);
}
void Explorer::imp_openMainMenu(){
	ui->removeGUIs();
	renderer->shutdown();

	screen = Screen::MAIN_MENU;
	renderer->init(GL::V1_1);
	begin();

	auto mmg = mkShared<MainMenuUI>(*this);
	mmg->create();
	ui->addPanel(mmg, 0);

	registerBlocks();
}

void Explorer::imp_joinGame(const std::string& addr, uint16 port, const std::string& name){
	log("Game", "Building...");
	gameInstance = mkUnique<Game>(*this);
	gameInstance->name = name;
	ui->getPanel("main_menu")->setVisible(false);
	gameInstance->init();
	gameInstance->gameFocused = true;
	if(gameInstance->connect(addr, port)){
		gameInstance->start();
		gameInstance->net_join();
	}

	renderer->window->holdCursor();
}

void Explorer::imp_quitGame(){
	if(gameInstance){
		gameInstance->stop();
		explorerInstance->universes.clear();
		gameInstance->disconnect();
		gameInstance->shutdown();
		gameInstance.reset();
		ui->getPanel("main_menu")->setVisible(true);
	}

	renderer->window->freeCursor();
}

void Explorer::shutdown(){
	log("Explorer", "Shutting down...");
	if(gameInstance){
		imp_quitGame();
		gameInstance.reset();
	}
}

void Explorer::imp_localCommand(const std::string& str){
	if(str.empty()) return;
	std::istringstream is(str);
	std::string s;
	std::vector<std::string> args;
	while(std::getline(is, s, ' ')){
		args.emplace_back(s);
	}
	auto& c = args[0];
	auto it = commands.find(c);
	if(it != commands.end()){
		it->second(args);
		return;
	}
	if("game" == c){
		auto& cmd = args[1];
		if("stop" == cmd){
			gameInstance->stop();
		} else if("disconnect" == cmd || "dc" == cmd){
			gameInstance->disconnect();
		} else if("shutdown" == cmd || "sd" == cmd){
			gameInstance->shutdown();
		}
	} else if("disconnect" == c || "dc" == c){
		if(gameInstance){
			quitGame();
		} else{
			log("Console", "No game to disconnect!");
		}
	} else if("exit" == c){
		renderer->window->setCloseFlag(true);
	} else if("uloadtex" == c){
		auto it = renderer->textures.find(args[1]);
		if(it == renderer->textures.end()){
			log("Console", "No such texture to unload!");
		} else{
			renderer->textures.erase(it);
		}
	} else if("loadtex" == c){
		renderer->loadTexture(args[1].c_str());
	} else if("reloadtex" == c){
		auto it = renderer->textures.find(args[1]);
		if(it == renderer->textures.end()){
			log("Console", "No such texture to unload!");
		} else{
			renderer->textures.erase(it);
		}
		renderer->loadTexture(args[1].c_str());
	} else if("list" == c){
		auto& type = args[1];
		if("gui" == type){
			log("Console", "GUIs attached:");
			for(auto& gn : ui->guiNames){
				log("+ " + gn);
				auto gui = ui->guis[gn];
				for(auto& c : gui->root->children){
					if(!c->name.empty()){
						log("  - " + c->name);
					}
				}
			}
		} else if("tex" == type){
			log("Console", "Textures loaded:");
			for(auto it = renderer->textures.begin(); it != renderer->textures.end(); ++it){
				log("+ " + it->first);
			}
		} else if("chunk" == type){
			//auto& reqs = args[2];
			//if(reqs == "-u"){
				if(gameInstance){
					for(const Chunk* ch : gameInstance->planet->chunkSet){
						std::ostringstream oss;
						oss << "[" << ch->cx << ", " << ch->cz << "] ";
						switch(ch->state){
						case Chunk::State::REQUESTED:
							oss << "R";
							break;
						case Chunk::State::FINISHED:
							oss << "F";
							break;
						}
						log(oss.str());
					}
				}
			//}
		}
	} else{
		log("Console", "Unknown command '" + c + "'!");
	}
}

void Explorer::regTask(std::function<void()> f){
	impl.tasks.emplace_back(f);
}

void Explorer::regCmd(string name, Command f){
	commands.emplace(name, f);
}

void Explorer::updateDebugText(){
	auto c = ui->getPanel("debug_text");
	if(!c) return;

	auto& text = ((Label*)c.get())->text;

	std::ostringstream ss;
	ss << "Explorer Alpha v" << E_VERSION_STR << " build " << E_BUILD_STR;
	ss << "\n$rFPS: $a" << fps;
	if(gameInstance){
		constexpr auto ogn = [](float n){ // Output green number
			std::ostringstream oss;
			oss << "$a" << n << "$r";
			return oss.str();
		};

		auto& gi = *gameInstance;
		auto& pos = gi.camera->pos;
		auto& rot = gi.camera->rot;
		ss << "\n$rP: " << ogn(pos.x) << " / " << ogn(pos.y) << " / " << ogn(pos.z) << " [" << ogn(rot.x) << ", " << ogn(rot.y) << "]";
		if(gi.planet) {
			ss << "\n$rTime: " << gi.planet->dayTime;
		}
		
	}
	text = ss.str();
}

void Explorer::log(const std::string& str){
	std::cout << str << "\n";
	consoleLog->addLine(str);
	auto con = console.lock();
	if(con) con->log(str + '\n');
}

void Explorer::log(const std::string& cl, const std::string& str){
	std::stringstream ss;
	ss << "[" << cl << "] " << str;
	log(ss.str());
}
