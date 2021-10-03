#include "Game.h"
#include "GameImpl.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "Game/Event.h"
#include "Game/TextLog.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/SplitPane.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/BorderPane.h"
#include "GUI/Components/TextField.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Keys.h"
#include "Render/Renderer.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/BufferedSocket.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/thread.h"
#include "ilib/logging.h"
#include "cwarns.h"
#include <iostream>
#include <cmath>
#include <cstring>
#define RADS (3.14159265359/180.0)

Game::Game(Explorer& exp)
: explorer(exp), impl_(new GameImpl(*this)), impl(*impl_), chatLog(new TextLog()), camera(new Camera()){
	lastMX = -1;
	lastMY = -1;
	camera->pos.y = 50;
	camera->rot.x = 90;
	log("Game", "Built!");
}

Game::~Game(){
	unregisterListeners();
	log("Game", "Destroyed.");
}

void Game::init(){
	log("Game", "Initializing...");
	menuGUI = makePauseMenu();
	explorer.ui->addPanel(menuGUI, 0);

	chatGUI = makeChatPanel();
	explorer.ui->addPanel(chatGUI, 1);

	registerListeners();
	log("Game", "Init Ok!");
}

bool Game::connect(std::string ip, uint16 port){
	log("Game", "Connecting to " + ip + ":" + std::to_string(port));
	Socket* _socket = nullptr;
	try {
		_socket = new Socket(ip.c_str(), port);
	} catch (const SocketException& se){
		log("Game", "Connection failed!");
		return false;
	}
	socket = new BufferedSocket(_socket);
	socket->init(16384);
	log("Game", "Connected.");
	return true;
}

void Game::start(){
	using namespace std::chrono_literals;

	log("Game", "Starting...");
	impl.batchingThread.reset(new Thread([this](){
		auto& queue = impl.chunkBatchingQueue;

		decltype(impl.chunkBatchingQueue) set; // Declares set to be the same type as queue
		while(!impl.batchingThread->getInterruptFlag()){
			{
				std::unique_lock<std::mutex> lock(impl.chunkAvailableLock);
				impl.chunkAvailableCV.wait_for(lock, 500ms);
				if(queue.empty()) continue;

				set = queue;
				queue.clear();
			}
			for(auto& ch : set){
				switch(ch->code){
				case 0:
					ch->chunk->batch();
					break;
				case 1:
					ch->chunk->batchCenter();
					break;
				case 2:
					ch->chunk->batchCorners();
					break;
				}
				ch->chunk->batchingOpQueue--;
				delete ch;
			}
		}
	}));
	log("Game", "Started.");
}

void Game::stop(){
	if(impl.batchingThread){
		impl.batchingThread->interrupt();
		impl.batchingThread->join();
	}
	impl.batchingThread = nullptr;
	log("Game", "Stopped.");
}

void Game::disconnect(){
	delete socket;
	socket = nullptr;
	log("Game", "Disconnected.");
}

void Game::shutdown(){
	unregisterListeners();
	explorer.ui->removePanel("game_pause_menu");
	explorer.ui->removePanel("game_chat");
	log("Game", "Shutdown.");
}

void Game::registerListeners(){
	Window& win = *explorer.renderer->window;
	keyListener = win.addKeyListener([this](int key, int, int act, int){
		if (!gameFocused) return;

		if(key == IKEY_ESCAPE && act == 1){
			if(menuOpen){
				closeMenu();
			} else if(chatOpen){
				closeChat();
			} else {
				openMenu();
			}
		}
		if(chatOpen){
			if(key == IKEY_ENTER && act == 1){
				if(isConnected()){
					sendChatMessage();
					chatInputField->text = "";
				}
			}
		} else if (key == IKEY_T && act) {
			openChat();
		}
	});

	mouseButtonListener = win.addMouseButtonListener([this](int btn, int act, int mods){

	});
	mouseMotionListener = win.addMouseMotionListener([this](double mx, double my){
		if (!gameFocused) return;
		if(lastMX != -1 && !uiOpen){
			auto& rot = camera->rot;
			rot.x += (my - lastMY) * 1;
			rot.y += (mx - lastMX) * 1;

			if(rot.x < -90) rot.x = -90;
			if(rot.x > 90) rot.x = 90;
		}
		lastMX = mx;
		lastMY = my;
	});
}
void Game::unregisterListeners(){
	if(!areListenersRegistered()) return;

	Window& win = *explorer.renderer->window;
	win.removeMouseButtonListener(mouseButtonListener);
	win.removeMouseMotionListener(mouseMotionListener);
	win.removeKeyListener(keyListener);
	win.removeCharListener(charListener);
}
bool Game::areListenersRegistered(){
	return keyListener != nullptr;
}
bool Game::isConnected(){
	return socket != nullptr;
}

bool Game::inPlanet(){
	return planet != nullptr;
}

void Game::update(double timeDelta){
	auto& win = *explorer.renderer->window;
	auto& cm = *camera;
	float speed = timeDelta * 60 * 1;

	if(gameFocused && !chatOpen){
		if(win.getKey(IKEY_A)){
			cm.pos.x -= cos(cm.rot.y * RADS) * speed;
			cm.pos.z -= sin(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(IKEY_D)){
			cm.pos.x += cos(cm.rot.y * RADS) * speed;
			cm.pos.z += sin(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(IKEY_W)){
			cm.pos.x += sin(cm.rot.y * RADS) * speed;
			cm.pos.z -= cos(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(IKEY_S)){
			cm.pos.x -= sin(cm.rot.y * RADS) * speed;
			cm.pos.z += cos(cm.rot.y * RADS) * speed;
		}
		if(win.getKey(IKEY_SPACE)){
			cm.pos.y += speed;
		}
		if(win.getKey(IKEY_LEFT_SHIFT)){
			cm.pos.y -= speed;
		}
	}

	if(!isConnected()) return;

	while(true){
		net_receive();
		if(net_inPacket.opcode){
			net_process();
		} else break;
	}

	if(!inPlanet()) return;
	auto& pl = *planet;

	const int rd = renderDistance;
	const int cx = floor(cm.pos.x / 24);
	const int cz = floor(cm.pos.z / 24);
	std::vector<uint64> chunksToDelete;
	if(impl.batchingThread){
		for(auto it = pl.chunkSet.begin(); it != pl.chunkSet.end(); ++it){
			auto* ch = *it;

			float lim = rd + 1;
			int difx = cx - ch->cx;
			int difz = cz - ch->cz;

			if(sqrt(difx*difx + difz*difz) > lim){
				std::lock_guard lock(ch->stateLock);
				if(ch->isDeletable()){
					ch->deleteFlag = true;
					chunksToDelete.emplace_back(ch->id);
				}
			}
		}
		for(uint64& id : chunksToDelete){
			pl.deleteChunk(id);
		}
	}

	for(int rx = -rd; rx < rd; rx++){
		for(int rz = -rd; rz < rd; rz++){
			if(sqrt(rx*rx + rz*rz) > rd) continue;
			Chunk* ch = pl.getChunk(cx + rx, 0, cz + rz);
			if(!ch){
				auto* chunk = pl.createChunk(cx + rx, 0, cz + rz);
				net_requestChunk(chunk->id);
				chunk->state = Chunk::State::REQUESTED;
			}
		}
	}
}

void Game::openMenu(){
	uiOpen = true;
	menuOpen = true;
	explorer.renderer->window->freeCursor();
	menuGUI->setVisible(true);
}

void Game::closeMenu(){
	uiOpen = false;
	menuOpen = false;
	explorer.renderer->window->holdCursor();
	chatGUI->setVisible(false);
	menuGUI->setVisible(false);
}

void Game::openChat(){
	uiOpen = true;
	chatOpen = true;
	chatGUI->setVisible(true);
	explorer.renderer->window->freeCursor();
	explorer.regTask(std::bind(&TextField::requestFocus, chatInputField.get()));
}

void Game::sendChatMessage(){
	if(chatInputField->text.length() > 0){
		net_sendChatMessage();
	}
}

void Game::closeChat(){
	uiOpen = false;
	chatOpen = false;
	chatOpening = false;
	explorer.renderer->window->holdCursor();
	chatInputField->text = "";
	chatInputField->releaseFocus();
	chatGUI->setVisible(false);
}

Shared<Component> Game::makePauseMenu(){
	auto& ui = *explorer.ui;
	auto root = Sh<Component>(Sh<StackLayout>());
	root->name = "game_pause_menu";
	root->setBackground({0.5f, 0.7f, 1, 0.2f});
	root->setVisible(false);

	auto menuRoot = root->addNew<BorderPane>();
	menuRoot->setBackground(false);
	menuRoot->setOrientation(0, Orientation::RIGHT_MIDDLE);
	menuRoot->setGrowRule(0, 0.25, 1.0);

	auto sideMenu = menuRoot->addNew<GridPane>();
	sideMenu->direction = GridPane::LR_BT;
	sideMenu->setGrid(1, 10);
	sideMenu->setSpacing(0, 8);
	sideMenu->setBackground({0.5f, 0.7f, 1, 1});
	sideMenu->setBackground(ui.sprites["panel"]);
	sideMenu->insets = {16, 16, 16, 16};

	sideMenu->add(ui.clButton("Resume", [this](auto&){
		closeMenu();
	}));
	sideMenu->add(ui.clButton("Quit Game", [this](auto&){
		explorer.quitGame();
	}));
	sideMenu->add(ui.clButton("Options", [root](auto&){
		root->children[1]->setVisible(true);
	}));

	auto optionsRoot = root->addT(Sh<BorderPane>());
	optionsRoot->setBackground({0.0, 0.7f, 1, 0.2f});
	optionsRoot->setVisible(false);

	{
		auto optionsWindow = optionsRoot->addT(Sh<BorderPane>());
		optionsWindow->setBackground(ui.sprites["panel"]);
		auto optionsTitle = optionsWindow->addT(ui.clLabel("Options"));
		optionsTitle->height = 30;
		optionsWindow->setOrientation(0, Orientation::CENTER_TOP);
		optionsWindow->setGrowRule(0, 1, 0);

		auto optionsGrid = optionsWindow->addT(Sh<GridPane>());
		optionsWindow->setOrientation(1, Orientation::CENTER_MIDDLE);
		optionsWindow->setGrowRule(1, 1, 1);
		optionsGrid->insets = {8, 38, 8, 8};
		optionsGrid->setSpacing(4, 4);
		optionsGrid->setBackground(false);
		optionsGrid->setGrid(1, 2);
		optionsGrid->direction = GridPane::LR_BT;
		auto returnBtn = optionsGrid->addT(ui.clButton("Return"));
		returnBtn->addActionListener([optionsRoot](auto&){
			optionsRoot->setVisible(false);
		});	
		auto colorsBtn = optionsGrid->addT(ui.clButton("Colors: ON"));
		colorsBtn->addActionListener([this](auto&){
			explorer.renderer->chunkColors = !explorer.renderer->chunkColors;
		});	

		optionsRoot->setOrientation(0, Orientation::CENTER_MIDDLE);
		optionsRoot->setGrowRule(0, 0.5, 0.5);
	}
	return root;
}

Shared<Component> Game::makeChatPanel(){
	auto root = Sh<BorderPane>();
	root->setVisible(false);
	root->name = "game_chat";
	root->setBackground({0, 0, 0, 0});
	root->insets = {5, 5, 5, 5};

	chatInputField = root->addT(explorer.ui->clTextField(""));
	chatInputField->insets = {3, 3, 3, 3};
	chatInputField->textOrientation = Orientation::LEFT_MIDDLE;
	chatInputField->setBounds(0, 0, 0, 28);
	chatInputField->setBackground({0.4f, 0.4f, 0.4f, 1});
	chatInputField->bgSprite.reset();
	root->setOrientation(0, Orientation::CENTER_BOTTOM);
	root->setGrowRule(0, 1.0, 0.0);

	auto chatLog_ = root->addNew<Component>(Sh<StackLayout>());
	chatLog_->setBackground(false);
	chatLog_->insets = {0, 0, 0, 35};
	chatLogLabel = chatLog_->addT(explorer.ui->clLabel(""));
	chatLogLabel->setBackground(false);
	chatLogLabel->textOrientation = Orientation::LEFT_BOTTOM;
	chatLogLabel->textSize = 2;
	root->setOrientation(1, Orientation::CENTER_MIDDLE);
	root->setGrowRule(1, 1.0, 1.0);
	return root;
}

void Game::net_requestChunk(uint64 id){
	net_outPacket.op(PacketCode::CHUNK);
	net_outPacket.data = (byte*)&id;
	net_outPacket.length = 8;
	net_send();
}

void Game::net_sendChatMessage(){
	net_outPacket.op(PacketCode::CHAT_MSG);
	auto& text = chatInputField->text;
	net_outPacket.data = (byte*)text.c_str();
	net_outPacket.length = text.length();
	net_send();
}

void Game::net_join(){
	net_outPacket.op(PacketCode::JOIN);
	net_outPacket.data = (byte*)name.c_str();
	net_outPacket.length = name.length();
	net_send();

	auto universe = mkShared<Universe>(explorer.explorerInstance.get());
	auto planet = mkShared<Planet>(universe.get());

	universe->planets.emplace_back(planet);
	explorer.explorerInstance->universes.emplace_back(universe);
	this->universe = universe.get();
	this->planet = planet.get();
}

bool Game::net_receive(){
	socket->clearup();
	socket->read(nullptr, 0);
	uint32 available = socket->available();
	if(available >= 6){
		byte* readHead = socket->buffer + socket->readPtr;

		uint32 len = *((uint32*)(readHead + 2));
		if(available >= len + 6){
			socket->read((byte*)&net_inPacket.opcode, 2);
			socket->read((byte*)&net_inPacket.length, 4);
			net_inPacket.data = new byte[len];
			socket->read(net_inPacket.data, len);
			return true;
		}
	}
	net_inPacket.opcode = 0;
	return false;
}
void Game::net_process(){
	if(net_inPacket.opcode == 0) return;
	Packet& packet = net_inPacket;

	switch((PacketCode)packet.opcode){
	case PacketCode::CHUNK: {
		auto id = *((uint64*)packet.data);

		auto* chunk = planet->getChunk(id);
		if(chunk && chunk->state == Chunk::State::REQUESTED) {
			chunk->allocateBlocks();
			memcpy((byte*)chunk->blocks, packet.data + 8, 24 * 24 * 24);
			chunk->state = Chunk::State::FINISHED;
			Chunk* nbs[8]{};
			planet->getChunkNeighbours8(*chunk, nbs);
			
			std::lock_guard lock(impl.chunkAvailableLock);
			chunk->batchingOpQueue++;
			impl.chunkBatchingQueue.emplace(new BatchRequest{0, chunk});
			for(int i = 0; i < 8; i++){
				if(Chunk* nb = nbs[i]){
					if(nb->blocks){
						nb->batchingOpQueue++;
						impl.chunkBatchingQueue.emplace(new BatchRequest{2, nb});
					}
				}
			}
			impl.chunkAvailableCV.notify_all();
		}
		}
		break;
	case PacketCode::CHAT_MSG: {
		auto msg = packet.asString();
		chatLog->addLine(msg);
		chatLogLabel->text = chatLog->asString();
		std::cout << "Chat: " << msg << "\n";
		}
		break;
	default:
		std::cout << "UP!\n";
	}

	delete packet.data;
	packet.data = nullptr;
}
void Game::net_send(){
	net_outPacket.send(socket);
}

void Game::log(const std::string& cl, const std::string& msg){
	explorer.log(cl, msg);
}
