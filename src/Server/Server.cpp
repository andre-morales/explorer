#include "Server.h"
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
#include <iostream>
#include <cstring>

int main(){
	std::cout << "Starting up...\n";
	Server* sv = new Server();
	sv->init();
	sv->run();
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

	/*Planet& pl = *planet;
	for(int cx = -1; cx <= 1; cx++){
		for(int cz = -1; cz <= 1; cz++){
			Chunk* ch = pl.getChunk(cx, 0, cz);
			if(!ch){
				auto* chunk = pl.allocateChunk(cx, 0, cz);
				chunk->gen();
			}
		}
	}*/

	Net::init();
	svSocket = mkUnique<TcpServer>(3333);
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

void Server::run(){
	svSocket->start();
	std::cout << "Listening for incoming connections.\n";

	while(true){
		auto* clientPtr = new ServerClient();
		clientPtr->socket = svSocket->acceptConnection();
		std::cout << "Incoming connection.\n";
		clientPtr->thread = new std::thread([this, clientPtr](){
			Planet& pl = *(explorerInstance->universes[0]->planets[0]);
			ServerClient& client = *clientPtr;
			auto& cs = *client.socket;
			client.socket->setBlocking(true);

			Packet in;
			byte* inOpcode = (byte*)&in.opcode;
			byte* inLength = (byte*)&in.length;

			Packet out;

			try {
				while(true){
					ASocket::spinReadEx(cs, inOpcode, 2);
					ASocket::spinReadEx(cs, inLength, 4);
					in.data = new byte[in.length];
					ASocket::spinReadEx(cs, in.data, in.length);

					switch((PacketCode)in.opcode){
					case PacketCode::JOIN:
						client.name = std::string((char*)in.data, in.length);

						//out.op(PacketCode::JOIN);
						//out.data = (byte*)"Connected.";
						//out.length = strlen((char*)out.data);
						//out.send(cstream);
						break;
					case PacketCode::CHAT_MSG: {
						std::string message = std::string("<") + client.name + "> " + std::string((char*)in.data, in.length);
						out.op(PacketCode::CHAT_MSG);
						out.data = (byte*)message.c_str();
						out.length = message.length();

						std::cout << "Chat: " << message << "\n";
						for(auto* pl : clients){
							out.send(pl->socket);
						}
						}
						break;
					case PacketCode::CHUNK: {
					    uint64 chunkId = *((uint64*)in.data);
						Chunk* chunk = pl.getChunk(chunkId);
						if(!chunk){
							chunk = pl.createChunk(chunkId);
							chunk->gen();
						}

						out.op(PacketCode::CHUNK);
						out.length = 8 + 24 * 24 * 24;
						out.data = new byte[out.length];
						memcpy(out.data, &chunkId, 8);
						memcpy(out.data + 8, chunk->blocks, 24 * 24 * 24);
						out.send(client.socket);
						delete out.data;

						}
						break;
					default:
						printPacket(in);
						break;
					}
				}
			} catch(const SocketException& se){
				std::cout << "Connection closed.\n";
			}
		});
		clients.emplace_back(clientPtr);
	}
}
