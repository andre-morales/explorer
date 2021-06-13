#include "Server.h"
#include "Game/Instance.h"
#include "Net/Packet.h"
#include "ilib/Net/TcpServer.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/ASocket.h"
#include "ilib/Net/SocketException.h"
#include <iostream>
#include <thread>
#include <vector>

int main(){
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

	Net::init();
	svSocket = mkUnique<TcpServer>(3333);
}
std::string tosdtr(byte* ptr, int len){
	std::string a("[");
	for(int i = 0; i < len; i++){
		a += std::to_string(ptr[i]);
		if(i < len - 1){
			a += std::string(", ");
		}

	}
	a += "]";
	return a;
}
void Server::run(){
	svSocket->start();
	byte buffer[512];

	std::vector<std::thread*> clients;

	while(true){
		Socket* cs = svSocket->acceptConnection();
        std::cout << "Connection.\n";

        auto* th = new std::thread([this, cs](){
            cs->setBlocking(true);
            Packet packet;

            try {
                while(true){
                    ASocket::spinReadEx(*cs, (byte*)&packet.opcode, 2);
                    ASocket::spinReadEx(*cs, (byte*)&packet.length, 4);
                    packet.data = new byte[packet.length];
                    ASocket::spinReadEx(*cs, packet.data, packet.length);
                }
            } catch(const SocketException& se){
                std::cout << "Connection closed.\n";
            }
        });
        clients.emplace_back(th);
	}
}
