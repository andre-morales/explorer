#include "Server.h"
#include "Server/Client.h"
#include "Game/Instance.h"
#include "Game/Universe.h"
#include "Game/Planet.h"
#include "Game/Chunk.h"
#include "Net/Packet.h"
#include "Net/PacketCodes.h"
#include "ilib/Net/TcpServer.h"
#include "ilib/Net/ASocket.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/IO/Stream.h"
#include "ilib/thread.h"
#include <iostream>
#include <cstring>

int main(){
	std::cout << "Starting up...\n";
	{
		Server sv{};
		sv.init();
		sv.run();
		sv.stop();
	}
	std::cin.ignore();
}

Server::Server(){

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
				auto* ch = pl.getChunk(req->id);
				if(!ch){
					ch = pl.createChunk(req->id);
					ch->gen();
				}
				auto packet = Sh<Packet>();
				createChunkPacket(*packet.get(), ch);
				packet->setDeleteFlag();
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

	std::cout << "Listening for incoming connections.\n";

	while(!clientListenerThread->interruptFlag){
		Socket* newSocket = serverSocket.acceptConnection();;
		std::cout << "Incoming connection.\n";

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

	try {
		while(!client.disconnected){
			std::this_thread::sleep_for(1ms);
			if(!waitingPacket){ // Empty
				if(socket.available() >= 6){
					ASocket::spinReadEx(socket, (byte*)&in.opcode, 2);
					ASocket::spinReadEx(socket, (byte*)&in.length, 4);
					
					if(in.length == 0){
						in.data = nullptr;
						handleClientPacket(client, &socket, in);
					} else {
						waitingPacket = true;
					}
				}
			} else if(socket.available() >= in.length){
				in.data = new byte[in.length];
				std::unique_ptr<byte[]> data(in.data); // Garantees that our data will even if exceptions are thrown.
				ASocket::spinReadEx(socket, in.data, in.length);

				waitingPacket = false;
				handleClientPacket(client, &socket, in);	
			}
			handleClientOutQueue(client);
			
		}
	} catch(const SocketException& se){
		std::cout << "Connection closed.\n";
	}

}

void Server::handleClientOutQueue(Client& client) {
	// Forward all queued packets.
	client.outQueueLock.lock();
	auto packets(client.outQueue); // Copy queue
	client.outQueue.clear();
	client.outQueueLock.unlock();

	for(auto& shpacket : packets){
		shpacket->send(client.socket);
	}
	
}

void Server::handleClientPacket(Client& client, Socket* socket, Packet& in){
	switch(in.opcode){
	case PacketCode::JOIN:
		client.name = std::string((char*)in.data, in.length);
		net_sendChatMessage("$aPlayer '$e" + client.name + "$a' joined!$r");
		break;
	case PacketCode::CHAT_MSG:
		net_sendChatMessage("<" + client.name + "> " + std::string((char*)in.data, in.length));
		break;
	case PacketCode::CHUNK: {
		Planet& pl = *(explorerInstance->universes[0]->planets[0]);
		uint64 chunkId = *((uint64*)in.data);
		Chunk* chunk = pl.getChunk(chunkId);

		if(chunk){
			Packet out;
			createChunkPacket(out, chunk);
			out.send(socket);
			delete[] out.data;
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
	in.data = 0;
}

void Server::net_sendChatMessage (const std::string& msg) {
	std::cout << "Chat: " << msg << "\n";

	auto packet = Sh<Packet>();
	packet->setDeleteFlag();

	createChatMessagePacket(*packet, msg);
	broadcastPacket(packet);
}

void Server::broadcastPacket(sh<Packet>& pkt){
	for(auto& pl : clients){
		pl->enqueue_sync(pkt);
	}
}

void Server::createChatMessagePacket (Packet& pkt, const std::string& msg) {
	pkt.opcode = PacketCode::CHAT_MSG;
	pkt.copyFullData(msg.c_str(), msg.length());
}

void Server::createChunkPacket(Packet& pkt, Chunk* c){
	pkt.opcode = PacketCode::CHUNK;
	pkt.length = 8 + 24 * 24 * 24;
	pkt.data = new byte[pkt.length];

	memcpy(pkt.data, &c->id, 8);
	memcpy(pkt.data + 8, c->blocks, 24 * 24 * 24);
}