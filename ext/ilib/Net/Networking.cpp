#include "Networking.h"
#include <winsock2.h>
#include "Exception.h"

bool Net::initialized = false;
WSAData* Net::wsa;

void Net::init(){
	wsa = new WSAData();
	initialized = true;
	if(WSAStartup(MAKEWORD(2, 2), wsa) != 0){
		throw Exception("WinSock failed to initialize!");
	}
}

void Net::release(){

}
