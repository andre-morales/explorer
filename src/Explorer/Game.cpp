#include "Game.h"
#include "GameImpl.h"
#include "Explorer/Explorer.h"
#include "Explorer/ExplorerGUI.h"
#include "Explorer/Camera.h"
#include "Explorer/Window.h"
#include "Explorer/ClientChunk.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/TextLog.h"
#include "GUI/Components/Component.h"
#include "GUI/Components/Label.h"
#include "GUI/Components/Button.h"
#include "GUI/Components/GridPane.h"
#include "GUI/Components/BorderPane.h"
#include "GUI/Components/TextField.h"
#include "GUI/Components/Toggle.h"
#include "GUI/Components/ScrollBar.h"
#include "GUI/Layouts/StackLayout.h"
#include "GUI/GUI.h"
#include "GUI/Sprite.h"
#include "GUI/Keys.h"
#include "Render/Renderer.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/Math/maths.h"
#include "ilib/IO/Compression/InflateStream.h"
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
	camera->rot.x = 0;
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
	try {
		impl.socket = new Socket(ip.c_str(), port);
		impl.socket->setBlocking(false);
		log("Game", "Connected.");
	} catch (const SocketException& se){
		log("Game", "Connection failed!");
		return false;
	}
	return true;
}

void Game::start(){
	using namespace std::chrono_literals;

	log("Game", "Starting...");
	impl.batchingThread.reset(new Thread());
	impl.batchingThread->thread = std::thread([this](){
		log("Game-BT", "Started.");
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
	});
	log("Game", "Started.");
}

void Game::stop(){
	if(impl.batchingThread){
		impl.batchingThread->interrupt();
		impl.batchingThread->join();
	}
	impl.batchingThread.reset();
	log("Game", "Stopped.");
}

void Game::disconnect(){
	net_disconnect();

	if(impl.socket){
		delete impl.socket;
		impl.socket = nullptr;
	}
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
	impl.keyListener = win.addKeyListener([this](int key, int, int act, int){
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

	impl.mouseButtonListener = win.addMouseButtonListener([this](int btn, int act, int mods){

	});
	impl.mouseMotionListener = win.addMouseMotionListener([this](double mx, double my){
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
	win.removeMouseButtonListener(impl.mouseButtonListener);
	win.removeMouseMotionListener(impl.mouseMotionListener);
	win.removeKeyListener(impl.keyListener);
	win.removeCharListener(impl.charListener);
}
bool Game::areListenersRegistered(){
	return impl.keyListener != nullptr;
}
bool Game::isConnected(){
	return impl.socket != nullptr;
}

bool Game::inPlanet(){
	return planet != nullptr;
}

void Game::update(double timeDelta){
	auto& win = *explorer.renderer->window;
	auto& cm = *camera;
	
	if(!inPlanet()) return;
	auto& pl = *planet;

	float speed = timeDelta * 60 * 1;
	if(win.getKey(IKEY_LEFT_CTRL)){
		speed *= 5;
	}

	if(pl.dayTime < 0) {
		pl.dayTime += (int)pl.dayTime + 1;

	}
	if(pl.dayTime > 1) {
		pl.dayTime -= (int)pl.dayTime;
	}
	if(gameFocused && !chatOpen){
		if(win.getKey(IKEY_O)){
			planet->dayTime -= 0.1 * timeDelta;
		} else if(win.getKey(IKEY_P)){
			planet->dayTime += 0.1 * timeDelta;
		}
		if(win.getKey(IKEY_K)){
			planet->sunXRotation -= 20 * timeDelta;
		} else if(win.getKey(IKEY_L)){
			planet->sunXRotation += 20 * timeDelta;
		}
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

	impl.processOutQueue();

	while(true){
		if(net_receive()){
			net_process();
		} else break;
	}

	const int rd = renderDistance;
	const int cx = floor(cm.pos.x / 24);
	const int cz = floor(cm.pos.z / 24);
	std::vector<Chunk*> chunksToDelete;
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
					chunksToDelete.emplace_back(ch);
				}
			}
		}
		for(Chunk*& ch : chunksToDelete){
			explorer.renderer->freeChunk(ch);
			pl.deleteChunk(ch->id);
		}
	}

	std::vector<Chunk*> chunksToRequest;

	for(int rx = -rd; rx < rd; rx++){
		for(int rz = -rd; rz < rd; rz++){
			if(sqrt(rx*rx + rz*rz) > rd) continue;

			Chunk* ch = pl.getChunk(cx + rx, 0, cz + rz);
			if(!ch) {
				ch = pl.createChunk(cx + rx, 0, cz + rz);
				chunksToRequest.push_back(ch);
			}
		}
	}

	std::sort(chunksToRequest.begin(), chunksToRequest.end(), [cx, cz](Chunk* a, Chunk* b) -> bool {
		int ax = a->cx - cx;
		int az = a->cz - cz;
		float ad = sqrt(ax*ax + az*az);

		int bx = b->cx - cx;
		int bz = b->cz - cz;
		float bd = sqrt(bx*bx + bz*bz);
		return ad < bd;
	});

	for(Chunk* ch : chunksToRequest) {
		net_requestChunk(ch->id);
		ch->state = Chunk::State::REQUESTED;
	}
}

void Game::onSecond () {
	downKiBps = impl.downBytesCounter / 1024.0;
	downBytesCounterFull += impl.downBytesCounter;
	impl.downBytesCounter = 0;

	upKiBps = impl.upBytesCounter / 1024.0;
	upBytesCounterFull += impl.upBytesCounter;
	impl.upBytesCounter = 0;
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

sh<Component> Game::makePauseMenu(){
	auto& rend = *explorer.renderer;
	auto& ui = *explorer.ui;
	auto root = Sh<Component>(new StackLayout());
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
		optionsTitle->setBackground(true);
		optionsWindow->setOrientation(0, Orientation::CENTER_TOP);
		optionsWindow->setGrowRule(0, 1, 0);

		auto optionsGrid = optionsWindow->addT(Sh<GridPane>());
		optionsWindow->setOrientation(1, Orientation::CENTER_MIDDLE);
		optionsWindow->setGrowRule(1, 1, 1);
		optionsGrid->insets = {8, 38, 8, 8};
		optionsGrid->setSpacing(4, 4);
		optionsGrid->setBackground(false);
		optionsGrid->setGrid(1, 4);
		optionsGrid->direction = GridPane::LR_TB;
		
		auto rdScroll = ui.clScroll();
		rdScroll->min = 1;
		rdScroll->max = 30;
		rdScroll->increment = 1;
		
		rdScroll->addValueListener([this, rdScroll](float v){
			renderDistance = v;
			rdScroll->children[1]->text = "Render Distance: " + std::to_string((uint32)v);
		});
		optionsGrid->add(rdScroll);

		rdScroll->setValue(24);

		optionsGrid->addT(ui.clToggle(true, [&rend](auto t, bool c){
			rend.window->setVSync(c);
			t->text = "VSync: " + std::string(c?"ON":"OFF");
		}))->fireValueListeners();

		optionsGrid->addT(ui.clToggle(true, [&rend](auto t, bool c){
			rend.chunkColors = c;
			t->text = "Colors: " + std::string(c?"ON":"OFF");
		}))->fireValueListeners();

		optionsGrid->addT(ui.clToggle(true, [&rend](auto t, bool c){
			rend.chunkLighting = c;
			t->text = "Lighting: " + std::string(c?"ON":"OFF");
		}))->fireValueListeners();

		optionsGrid->add(ui.clButton("Return", [optionsRoot](auto&){
			optionsRoot->setVisible(false);
		}));
		optionsRoot->setOrientation(0, Orientation::CENTER_MIDDLE);
		optionsRoot->setGrowRule(0, 0.5, 0.5);
	}
	return root;
}

sh<Component> Game::makeChatPanel(){
	auto root = Sh<BorderPane>();
	root->setVisible(false);
	root->name = "game_chat";
	root->setBackground({0, 0, 0, 0});
	root->insets = {5, 5, 5, 5};

	auto chatLog_ = root->addNew<Component>(new StackLayout());
	chatLog_->setBackground(false);
	chatLog_->insets = {0, 0, 0, 35};
	chatLogLabel = chatLog_->addT(explorer.ui->clLabel(""));
	chatLogLabel->setBackground(false);
	chatLogLabel->textOrientation = Orientation::LEFT_BOTTOM;
	chatLogLabel->textSize = 2;
	root->setOrientation(0, Orientation::CENTER_MIDDLE);
	root->setGrowRule(0, 1.0, 1.0);

	chatInputField = root->addT(explorer.ui->clTextField(""));
	chatInputField->insets = {3, 3, 3, 3};
	chatInputField->textOrientation = Orientation::LEFT_MIDDLE;
	chatInputField->setBounds(0, 0, 0, 28);
	chatInputField->setBackground({0.4f, 0.4f, 0.4f, 1});
	chatInputField->bgSprite.reset();
	root->setOrientation(1, Orientation::CENTER_BOTTOM);
	root->setGrowRule(1, 1.0, 0.0);
	return root;
}

void Game::net_requestChunk(uint64 id){
	byte buffer[6 + 8];
	net_outPacket.buffer = buffer;
	net_outPacket.opcode(PacketCode::CHUNK);
	net_outPacket.data_copy_len((byte*)&id, 8);
	net_outPacket.flags.stack_allocated = true;
	net_send();
	net_outPacket.release();
}

void Game::net_sendChatMessage(){
	auto& text = chatInputField->text;
	net_outPacket.create(PacketCode::CHAT_MSG, (byte*)text.c_str(), text.length());
	net_send();
	net_outPacket.destroy();
}

void Game::net_disconnect () {
	byte buffer[6];
	net_outPacket.buffer = buffer;
	net_outPacket.opcode(PacketCode::DISCONNECT);
	net_outPacket.length(0);
	net_outPacket.flags.stack_allocated = true;
	net_send();
	net_outPacket.release();
}

void Game::net_join(){
	log("Game", "Joining...");
	net_outPacket.create(PacketCode::JOIN, (byte*)name.c_str(), name.length());
	net_send();
	net_outPacket.destroy();

	log("Game", "Packet sent.");
	auto universe = Sh<Universe>(explorer.explorerInstance.get());
	auto planet = Sh<Planet>(universe.get());
	log("Game", "Built world.");

	universe->planets.emplace_back(planet);
	explorer.explorerInstance->universes.emplace_back(universe);
	this->universe = universe.get();
	this->planet = planet.get();
	log("Game", "Joined.");
}

bool Game::net_receive(){
	try {
		impl.downBytesCounter += impl.inflater->writeFrom(impl.socket, 32768);
		impl.inflater->write(nullptr, 0);

		uint32 toRead = impl.inflater->toRead();
		uint16* opcode_ptr = (uint16*)impl.readInHeader;
		uint32* len_ptr = (uint32*)(impl.readInHeader + 2);
		if(*opcode_ptr == 0 && toRead >= 6) {
			impl.inflater->read(impl.readInHeader, 6);

			uint16 opcode = *opcode_ptr;
			if(opcode == 0 || opcode > 5) throw Exception("Illegal opcode!");
			toRead -= 6;
		}

		if(*opcode_ptr != 0) {
			uint32 len = *len_ptr; // Peek at packet length
			if(toRead >= len){
				net_inPacket.buffer = new byte[6 + len];
				memcpy(net_inPacket.buffer, impl.readInHeader, 6);
				int wrr = impl.inflater->read(net_inPacket.buffer + 6, len);
				if(wrr != len) throw Exception("illegal!");
				*opcode_ptr = 0;
				return true;
			}
		}
	} catch (const SocketException& se){
		log("Game", se.message);
	}
	return false;
}


void Game::net_process(){
	if(!net_inPacket.buffer) return;
	Packet& packet = net_inPacket;
	uint16 opcode = packet.opcode();
	switch(opcode){
	case PacketCode::CHUNK: {
		auto id = *((uint64*)packet.data());

		auto* chunk = planet->getChunk(id);
		if(chunk && chunk->state == Chunk::State::REQUESTED) {
			chunk->allocateBlocks(false);
			memcpy((byte*)chunk->blocks, packet.data() + 8, 24 * 24 * 24);

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
		auto msg = std::string((char*)packet.data(), packet.length());
		chatLog->addLine(msg);
		chatLogLabel->text = chatLog->asString();
		std::cout << "Chat: " << msg << "\n";
		}
		break;
	case 0: break;
	default:
		throw Exception("Illegal packet code!");
	}

	packet.destroy();
}

void Game::net_send(){
	if(!impl.socket) return;
	Packet& p = net_outPacket;
	uint32 plen = p.length();
	try {
		impl.socket->write(p.buffer, 6 + plen);
		impl.upBytesCounter += plen + 6;
	} catch (const SocketException& se){
		if(se.code != SocketException::BUFFER_FULL) {
			throw se;
		} else {
			if(p.flags.stack_allocated) {
				// Packet has its buffer allocated on the stack, to enqueue it,
				// we'll have to copy its data. New copy has unique_buffer flag set
				// so that it gets deleted when the packet does finally gets sent.
				byte* copy = new byte[6 + plen];
				memcpy(copy, p.buffer, 6 + plen);
				impl.outQueue.emplace(copy, Packet::Flags{false, true});
			} else {
				impl.outQueue.emplace(p.buffer, p.flags);
			}
		}
	}
}

void Game::log(const std::string& cl, const std::string& msg){
	explorer.log(cl, msg);
}
