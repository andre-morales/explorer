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
#include "Game/TextLog.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/TextField.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/Keys.h"
#include "Render/GL.h"
#include "Render/Renderer.h"
#include "ilib/Net/Socket.h"
#include "ilib/itime.h"
#include "Exception.h"
#include <iostream>
#include <sstream>
#include <queue>
#include <fstream>
#include <filesystem>
#include "cwarns.h"

using std::string;

int main(){
	std::cout << "[-] Starting up...\n";
	try {
		{
			Explorer explorer{};
			explorer.init();
			explorer.run();
			explorer.shutdown();
		}
		std::cout << "[-] Bye!\n";
		return 0;
	} catch(const Exception& ex){
		std::cerr << "Explorer exception '" << ex.message << "'.\n";
	} catch(const std::exception& ex){
		std::cerr << "std::exception '" << ex.what() << "'.\n";
	} catch(...){
		std::cerr << "Unknown error.\n";
	}

	std::cin.ignore();
	return 1;
}

Explorer::Explorer() : impl_(new ExplorerImpl(*this)), impl(*impl_){
	log("Explorer", "Building...");
	if(std::filesystem::exists("__enable_client_log")) {
		log("Explorer", "Manual logging flag set.");
		
		createLoggingFile();
	}
	
	consoleLog.reset(new TextLog());
	
	renderer.reset(new Renderer(*this));
	log("Explorer", "Buit.");
}

Explorer::~Explorer(){
	if(impl.logFile) fclose(impl.logFile);
	log("Explorer", "Destroying...");
}

void Explorer::init(){
	log("Explorer", "Initializing...");
	renderer->init(GL::C1_1);
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
				if(impl.debugTextVisible = !impl.debugTextVisible) {
					renderer->window->setTitle("Explorer");
				}
				ui->getPanel("debug_text")->setVisible(impl.debugTextVisible);				
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

void Explorer::createLoggingFile () {
	loggingEnabled = true;
	if(impl.logFile) return; // Log file already created.

	log("Explorer", "Creating log file...");
	int num = std::chrono::duration_cast<std::chrono::microseconds>((timenow().time_since_epoch())).count() * 0.001;
	auto fileName = "explorerclient_log_" + std::to_string(num) + ".txt";
	impl.logFile = fopen(fileName.c_str(), "w+");
	if(!impl.logFile) throw Exception("Failed to open log file!");	
	log("Explorer", "Ok.");
}

void Explorer::begin(){
	renderer->loadResources();
	renderer->begin();
	ui.reset(new ExplorerGUI(*this));

	registerCommands();

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
	ei.registerBlock({"air",    0, 0.00, 0});
	ei.registerBlock({"stone",  1, 1.00, 1});
	ei.registerBlock({"grass",  2, 1.00, 2, 3, 4});
	ei.registerBlock({"dirt",   3, 1.00, 4});
	ei.registerBlock({"glass",  4, 0.40, 5});
	ei.registerBlock({"wood",   5, 1.00, 6});
	ei.registerBlock({"leaves", 6, 0.60, 7});
	for(uint16 i = 7; i < 256; i++){
		ei.registerBlock({"unknown", (byte)i, 0.1, (byte)i});
	}
	ei.registerBlock({"unknown", 0xEA, 0.1, 1});
	ei.registerBlock({"unknown", 0xCD, 1.0, 1});
}

void Explorer::run(){
	Net::init();

	auto now = timenow();
	auto ufTime = now; // Update frame time stamp
	auto ffTime = now; // FPS frame time stamp
	int frameCount = 0;

	while(!renderer->window->getCloseFlag()){
		now = timenow();
		frameTime = elapsedbetween(ufTime, now);
		ufTime = now;
		if(elapsedbetween(ffTime, now) >= 1){
			ffTime = now;
			fps = frameCount;
			frameCount = 0;

			if(!impl.debugTextVisible) {
				renderer->window->setTitle("Explorer FPS: " + std::to_string(fps));
			}
			if(gameInstance){
				gameInstance->onSecond();
			}
		}

		renderer->render();
		update(frameTime);
		auto swapTime_ = timenow();
		renderer->window->bufferSwap();
		swapTime = elapsedbetween(swapTime_, timenow());

		++frameCount;
	}
}

void Explorer::update(double timeDelta){
	Window::fireEvents();

	if(gameInstance){
		gameInstance->update(timeDelta);
	}
	if(impl.debugTextVisible) {
		auto c = ui->getPanel("debug_text");
		if(c){
			impl.updateDebugText();
			((Label*)c.get())->text = impl.debugText;		
		}
	}

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

void Explorer::exit() {
	regTask(&ExplorerImpl::exit, &impl);
}

void Explorer::registerCommands () {
	#define ESwitch(X) auto& SWITCH_ARG = X;
	#define ECase(X) if(SWITCH_ARG == X){
	#define EElseCase(X) } else if(SWITCH_ARG == X){
	#define EDefault } else {
	#define EBreak }

	regCmd("window", [&w_ = renderer->window](auto args) -> bool {
		if(args.size() < 2) return false;
		auto& cmd = args[1];
		auto& win = *w_;

		ESwitch(cmd)
		ECase("full")     win.makeFullscreen();
		EElseCase("win")  win.makeWindowed();
		EElseCase("hold") win.holdCursor();
		EElseCase("free") win.freeCursor();
		EDefault          return false;
		EBreak
		return true;
	});

	regCmd("tex", [this, &rd = *renderer](auto args) -> bool {
		auto size = args.size();
		if(size < 2) return false;

		auto& cmd = args[1];
		ESwitch(cmd)
		ECase("load")
			if(size == 3) {
				rd.loadTexture(args[2]);
			} else {
				rd.loadTexture(args[2], args[3]);
			}
		EElseCase("unload")
			if(size < 3) return false;

			auto it = rd.textures.find(args[2]);
			if(it == rd.textures.end()){
				log("Console", "No such texture to unload!");
			} else {
				rd.textures.erase(it);
			}
		EElseCase("list")
			log("Console", "Textures loaded:");
			for(auto& p : rd.textures){
				log("+ " + p.first);
			}
		EDefault return false;
		EBreak

		return true;
	});

	regCmd("shader", [this, &rd = *renderer](auto args) -> bool {
		auto size = args.size();
		if(size < 2) return false;

		auto& cmd = args[1];
		ESwitch(cmd)
		ECase("load")
			if(size == 3) {
				rd.loadShader(args[2]);
			} else {
				rd.loadShader(args[2], args[3]);
			}
		EDefault return false;
		EBreak
			return true;
	});
}

void Explorer::openMainMenu(uint32 gl){
	regTask(&Explorer::imp_openMainMenu, this, std::move(gl));
}
void Explorer::joinGame(const std::string& addr, uint16 port, const std::string& name){
	regTask(&Explorer::imp_joinGame, this, addr, std::move(port), name);
}
void Explorer::localCommand(const std::string& cmd){
	regTask(&Explorer::imp_localCommand, this, cmd);
}
void Explorer::quitGame(){
	regTask(&ExplorerImpl::quitGame, &impl);
}

void Explorer::imp_openMainMenu(uint32 gl){
	if(loggingEnabled) createLoggingFile();
	ui->removeGUIs();
	renderer->shutdown();

	screen = Screen::MAIN_MENU;
	GL::GLContext glv;
	switch(gl) {
		default:	
		case 0: glv = GL::C1_1; break;
		case 1: glv = GL::C1_5; break;
		case 2: glv = GL::C2_0; break;
	}
	renderer->init(glv);
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

void Explorer::shutdown(){
	log("Explorer", "Shutting down...");
	if(gameInstance){
		impl.quitGame();
		gameInstance.reset();
	}
	log("Explorer", "Done.");
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
	auto it = impl.commands.find(c);
	if(it != impl.commands.end()){
		if(!it->second(args)){
			log("Console", "Unknown subcommand or incorrect parameters.");
		}
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
	} else if("rd" == c) {
		if(!gameInstance) return;
		log("Explorer", "Changing render distance to " + args[1]);
		gameInstance->renderDistance = std::stoi(args[1]);
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
		}
	} else if("dbg_chst" == c) {
		renderer->debugChunkState = !renderer->debugChunkState;
	} else if("dbg_chdump" == c) {
		log("Console", "Dumping all blockdata to 'dbg_chunks'");
		for(auto& ch : gameInstance->planet->chunkSet) {
			if(ch->blocks){
				auto file = "dbg_chunks\\" + std::to_string(ch->cx) + "-" + std::to_string(ch->cz) + ".ec";
				std::ofstream off(file, std::ofstream::out);
				if(off.fail()) throw Exception("dbg_chunks failed.");
				off.write((char*)ch->blocks, 24 * 24 * 24);
				off.close();
			}
		}
	} else {
		log("Console", "Unknown command '" + c + "'!");
	}
}

void Explorer::regTask(std::function<void()> f){
	impl.tasks.emplace_back(f);
}

void Explorer::regCmd(string name, std::function<bool(std::vector<std::string>)> f){
	impl.commands.emplace(name, f);
}

void Explorer::log(const std::string& str){
	auto strln = str + "\n";

	std::lock_guard lock(impl.logLock);
	std::cout << strln; // Stdout

	if(impl.logFile){ // Log file
		fputs(strln.c_str(), impl.logFile);
		fflush(impl.logFile);
	}

	if(consoleLog) consoleLog->addLine(str); // Internal log str
	if(auto con = console.lock()) con->log(strln); // Console UI
}

void Explorer::log(const std::string& cl, const std::string& str){
	std::stringstream ss;
	ss << "[" << cl << "] " << str;
	log(ss.str());
}
