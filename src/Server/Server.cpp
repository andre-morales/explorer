#include "Server.h"
#include "Server/Version.h"
#include "Server/Client.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Server/ServerChunk.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/TcpServer.h"
#include "ilib/Net/ASocket.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/IO/Stream.h"
#include "ilib/thread.h"
#include "ilib/logging.h"
#include "ilib/Math/maths.h"
#include "ilib/IO/Compression/BZ2_CompressorStream.h"
#include "ilib/IO/Compression/ZL_CompressorStream.h"
#include "ilib/IO/Compression/BZ2_Compressor.h"
#include <iostream>
#include <cstring>
#include "ilib/IO/BitBuffer.h"
#include "yaml-cpp/yaml.h"
#include <filesystem>
#include <fstream>
#include "Net/PacketReader.h"
#include "ilib/IO/BitBuffer.h"
int main(){
	logln("Starting up...");
	{
		/*BitBuffer bb;
		uint32 numb = 0;
		bb.bufferSize = 4;
		bb.buffer = (byte*)&numb;
		bb.writeAlign = BitBuffer::BitGroupAlignment::LEFT_TO_RIGHT;
		bb.write(0b00000000, 2);
		bb.write(0b11000000, 2);
		bb.write(0b11100000, 4);
		std::cout << numb;*/
		
		//PacketReader pr;
		//CircularBuffer cb;
		
		//byte bytes[4] = {0x30, 0x02, 0x01, 0x81};
		//cb.write(bytes, 4);
		//pr.receive(&cb);

		Server sv{};
		sv.init();
		sv.run();
		sv.stop();
	}
	//std::cin.ignore();
}

Server::Server(){
	logln("Building Explorer Server " + std::string(ServerVersion::VER_STR));
}
Server::~Server(){
	delete config;
}

void Server::init(){
	loadConfig();

	explorerInstance = mkUnique<Instance>();
	auto& ei = *explorerInstance;

	auto universe = mkShared<Universe>(explorerInstance.get());
	auto planet = mkShared<Planet>(universe.get());
	universe->planets.emplace_back(planet);
	ei.universes.emplace_back(universe);

	Net::init();
}

void Server::loadConfig() {
	config = new YAML::Node();
	extern const char* const DEFAULT_CONFIG;

	if (!std::filesystem::exists("server.yml")) {
		logln("Configuration file not found! Creating with defaults...");
		std::ofstream configFile("server.yml");
		configFile << DEFAULT_CONFIG;
		configFile.close();
	}

	*config = YAML::LoadFile("server.yml");	
}

void Server::run(){
	clientListenerThread = new Thread(std::thread(&Server::clientListenerFn, this));
	worldGeneratorThread = new Thread(std::thread(&Server::worldGeneratorFn, this));
	while(true){
		std::cin.ignore();
	}
}

void Server::stop(){
	clientListenerThread->interruptAndWait();
}

void Server::worldGeneratorFn(){
	using namespace std::chrono_literals;

	auto& queue = worldGenQueue;
	decltype(worldGenQueue) set; // Declares set to be the same type as queue
	while(true){
		Planet& pl = *(explorerInstance->universes[0]->planets[0]);
		{
			std::unique_lock lock(worldGenLock);
			worldGenCV.wait_for(lock, 500ms);
			if(queue.empty()) continue;

			std::swap(set, queue);
		}
		while(!set.empty ()) {
			auto req = set.front();
			set.pop();
			switch(req->style){
			case 0:
				pl.lock.lock();
				auto* ch = pl.getChunk(req->id);
				if(!ch){
					ch = pl.createChunk(req->id);
					pl.lock.unlock();
					ch->allocateBlocks(true);
					ch->gen();
				} else {
					pl.lock.unlock();
				}
				auto packet = Sh<Packet>();
				createChunkPacket(*req->client, *packet.get(), ch);
				packet->flags.unique_buffer = true;
				req->client->enqueue_sync(packet);
				break;
			}
			delete req;
		}
	}
}

void Server::clientListenerFn(){
	TcpServer serverSocket{3333};
	serverSocket.start();

	log("Listening for incoming connections.\n");

	while(!clientListenerThread->interruptFlag){
		Socket* newSocket = serverSocket.acceptConnection();;
		log("Incoming connection.\n");

		auto* client = new Client();
		client->socket = newSocket;
		clients.emplace_back(client);
		client->thread = new std::thread(&Server::clientThreadFn, this, client);
	}
}

void Server::clientThreadFn(Client* client_){
	using namespace std::chrono_literals;

	auto& client = *client_;
	auto& socket = *client.socket;
	socket.setBlocking(true);

	Packet in;
	bool waitingPacket = false;
	byte inHeader[5];
	uint8& inOpcode = *(uint8*)inHeader;
	uint32& inLength = *(uint32*)(inHeader + 1);

	client.outBuffer = un<Stream>(new BZ2_CompressorStream(1024 * 32, 1024 * 32));
	try {
		while(!client.disconnected){
			//std::this_thread::sleep_for(200ms);
			if(waitingPacket && socket.toRead() >= inLength){
				in.buffer = new byte[inLength + 5];
				un<byte[]> data(in.buffer);
				memcpy(in.buffer, inHeader, 5);

				ASocket::spinReadEx(*client.socket, in.data(), inLength);
				handleClientPacket(client, in);	
				waitingPacket = false;
			}
			while(!waitingPacket && socket.toRead() >= 5) {
				socket.read(inHeader, 5);

				if(inLength == 0){
					in.buffer = inHeader;
					handleClientPacket(client, in);
				} else if(socket.toRead() >= inLength){
					in.buffer = new byte[inLength + 5];
					un<byte[]> data(in.buffer);
					memcpy(in.buffer, inHeader, 5);

					socket.read(in.data(), inLength);

					handleClientPacket(client, in);	
				} else {
					waitingPacket = true;
				}
			}

			handleClientOutQueue(client);
			
		}
	} catch(const SocketException& se){
		logln(se.message);
	}
	log("Connection closed.\n");
}

void Server::handleClientOutQueue(Client& client) {
	auto outBuffer = client.outBuffer.get();
	outBuffer->write(nullptr, 0);
	outBuffer->readInto(client.socket, UINT32_MAX);

	// Forward queued packets.
	std::unique_lock lock_(client.outQueueLock);
	if(client.outQueue.empty()) return;

	sh<Packet>& packet_ = client.outQueue.front();
	uint32 size = 5 + packet_->length();
	uint32 space = outBuffer->toWrite();
	if(space < size) return;

	sh<Packet> packet = packet_;
	client.outQueue.pop();
	lock_.unlock();

	outBuffer->write(packet->buffer, size);
}

void Server::handleClientPacket(Client& client, Packet& in){
	auto* socket = client.socket;
	auto opcode = in.opcode();
	switch(opcode){
	case PacketInfo::DISCONNECT:
		client.disconnected = true;
		net_sendChatMessage("$cPlayer '$e" + client.name + "$c' disconnected!$r");
		break;
	case PacketInfo::JOIN:
		client.name = std::string((char*)in.data(), in.length());
		net_sendChatMessage("$aPlayer '$e" + client.name + "$a' joined!$r");
		break;
	case PacketInfo::CHAT_MSG: {
		std::string msg((char*)in.data(), in.length());
		if(msg.empty()) break;

		if(msg[0] == '/') {
			if(!clientCommand(client, msg)){
				
			}
		} else {
			net_sendChatMessage("<" + client.name + "> " + msg);
		}	
		break;
	}
	case PacketInfo::CHUNK: {
		Planet& pl = *(explorerInstance->universes[0]->planets[0]);
		uint64 chunkId = *((uint64*)in.data());
		pl.lock.lock();
		Chunk* chunk = pl.getChunk(chunkId);
		pl.lock.unlock();

		if(chunk){
			auto packet = Sh<Packet>();
			createChunkPacket(client, *packet, chunk);
			packet->flags.unique_buffer = true;
			client.enqueue_sync(packet);
		} else {
			std::unique_lock lock(worldGenLock);
			worldGenQueue.push(new GenRequest{chunkId, 0, &client});
			worldGenCV.notify_one();
		}
	}
	break;
	}
	in.buffer = 0;
}

void Server::net_sendChatMessage (const std::string& msg) {
	logln("Chat: " + msg);

	auto packet = Sh<Packet>();
	packet->flags.unique_buffer = true;

	createChatMessagePacket(*packet, msg);
	broadcastPacket(packet);
}

void Server::broadcastPacket(sh<Packet>& pkt){
	for(auto& pl : clients){
		pl->enqueue_sync(pkt);
	}
}

void Server::createChatMessagePacket (Packet& pkt, const std::string& msg) {
	pkt.create(PacketInfo::CHAT_MSG, (byte*)msg.c_str(), msg.length());
}

void Server::createChunkPacket(Client& cl, Packet& pkt, Chunk* c){
	BitBuffer* bb = c->toBitfield();
	pkt.create(PacketInfo::CHUNK, 8 + bb->bufferSize);
	memcpy(pkt.data(), &c->id, 8);
	memcpy(pkt.data() + 8, bb->buffer, bb->bufferSize);
}

bool Server::clientCommand(Client& client, const std::string& cmd) {
	std::cout << "Client sent command '" << cmd << "'\n";
	if(cmd == "/flush_deflator") {
		auto* outptr = client.outBuffer.get();
		
		BZ2_CompressorStream* outbz2 = dynamic_cast<BZ2_CompressorStream*>(outptr);
		if(outbz2) {
			do {
				outbz2->flush_sync();
				outbz2->readInto(client.socket, UINT32_MAX);
			} while(outbz2->inBuffer->toRead() || outbz2->outBuffer->toRead());
			byte nullp[] = {0x00};
			outbz2->write(nullp, 1);
			outbz2->flush_sync();
		}
		ZL_CompressorStream* outzl = dynamic_cast<ZL_CompressorStream*>(outptr);
		if(outzl) {
			do {
				outzl->flush_sync();
				outzl->readInto(client.socket, UINT32_MAX);
			} while(outzl->inBuffer->toRead() || outzl->outBuffer->toRead());
		}
		return true;
	} else return false;
}