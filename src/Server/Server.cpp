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
#include "ilib/IO/Compression/DeflateStream.h"
#include <iostream>
#include <cstring>

int main(){
	logln("Starting up...");
	{
		Server sv{};
		sv.init();
		sv.run();
		sv.stop();
	}
	std::cin.ignore();
}

Server::Server(){
	logln("Building Explorer Server " + std::string(ServerVersion::VER_STR));
}
Server::~Server(){

}

void Server::init(){
	explorerInstance = mkUnique<Instance>();
	auto& ei = *explorerInstance;

	auto universe = mkShared<Universe>(explorerInstance.get());
	auto planet = mkShared<Planet>(universe.get());
	universe->planets.emplace_back(planet);
	ei.universes.emplace_back(universe);

	Net::init();
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

			set = queue;
			queue.clear();
		}
		for(auto& req : set){
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
				createChunkPacket(*packet.get(), ch);
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
	byte inHeader[6];
	uint16& inOpcode = *(uint16*)inHeader;
	uint32& inLength = *(uint32*)(inHeader + 2);

	client.outBuffer = un<Stream>(new DeflateStream(1024 * 16, 1024 * 16));

	try {
		while(!client.disconnected){
			//std::this_thread::sleep_for(200ms);
			if(waitingPacket && socket.toRead() >= inLength){
				in.buffer = new byte[inLength + 6];
				un<byte[]> data(in.buffer);
				memcpy(in.buffer, inHeader, 6);

				ASocket::spinReadEx(*client.socket, in.data(), inLength);
				handleClientPacket(client, in);	
				waitingPacket = false;
			}
			while(!waitingPacket && socket.toRead() >= 6) {
				socket.read(inHeader, 6);

				if(inLength == 0){
					in.buffer = inHeader;
					handleClientPacket(client, in);
				} else if(socket.toRead() >= inLength){
					in.buffer = new byte[inLength + 6];
					un<byte[]> data(in.buffer);
					memcpy(in.buffer, inHeader, 6);

					int r = socket.read(in.data(), inLength);
					if(r < inLength) {
						std::cout << "misalign";
					}

					handleClientPacket(client, in);	
				} else {
					waitingPacket = true;
				}
			}
			handleClientOutQueue(client);
		}
	} catch(const SocketException& se){
	}
	log("Connection closed.\n");
}

void Server::handleClientOutQueue(Client& client) {
	byte compressdata[1024 * 8];

	client.outBuffer->write(nullptr, 0);
	uint32 cdw = client.outBuffer->read(compressdata, 1024 * 8);

	uint32 arr = client.socket->write(compressdata, cdw);
	if(arr != cdw) throw Exception("AAA");

	// Forward all queued packets.
	std::unique_lock lock_(client.outQueueLock);
	if(client.outQueue.empty()) return;

	auto begin = client.outQueue.begin();
	uint32 size = 6 + (*begin)->length();
	int space = client.outBuffer->toWrite();
	if(space < size) return;

	sh<Packet> packet = *begin;
	client.outQueue.erase(begin);
	lock_.unlock();

	packet->send(client.outBuffer.get());
}

void Server::handleClientPacket(Client& client, Packet& in){
	//std::cout << "hout\n";
	auto* socket = client.socket;
	auto opcode = in.opcode();
	switch(opcode){
	case PacketCode::JOIN:
		client.name = std::string((char*)in.data(), in.length());
		net_sendChatMessage("$aPlayer '$e" + client.name + "$a' joined!$r");
		break;
	case PacketCode::CHAT_MSG:
		net_sendChatMessage("<" + client.name + "> " + std::string((char*)in.data(), in.length()));
		break;
	case PacketCode::CHUNK: {
		Planet& pl = *(explorerInstance->universes[0]->planets[0]);
		uint64 chunkId = *((uint64*)in.data());
		pl.lock.lock();
		Chunk* chunk = pl.getChunk(chunkId);
		pl.lock.unlock();

		if(chunk){
			auto packet = Sh<Packet>();
			createChunkPacket(*packet, chunk);
			packet->flags.unique_buffer = true;
			client.enqueue_sync(packet);
		} else {
			std::unique_lock lock(worldGenLock);
			worldGenQueue.push_back(new GenRequest{chunkId, 0, &client});
			worldGenCV.notify_one();
		}
	}
	break;
	case PacketCode::DISCONNECT:
		client.disconnected = true;
		net_sendChatMessage("$cPlayer '$e" + client.name + "$c' disconnected!$r");
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
	pkt.create(PacketCode::CHAT_MSG, (byte*)msg.c_str(), msg.length());
}

void Server::createChunkPacket(Packet& pkt, Chunk* c){
	pkt.create(PacketCode::CHUNK, 8 + 24 * 24 * 24);
	memcpy(pkt.data(), &c->id, 8);
	memcpy(pkt.data() + 8, c->blocks, 24 * 24 * 24);
}