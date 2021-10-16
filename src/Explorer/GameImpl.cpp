#include "GameImpl.h"
#include "Game.h"
#include "ilib/IO/Compression/InflateStream.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/thread.h"

GameImpl::GameImpl(Game& g) : This(g), inflater(new InflateStream(16384, 16384)){

};
GameImpl::~GameImpl(){};

void GameImpl::processOutQueue () {
	if(!socket) return;
	while(!outQueue.empty ()) {
		const Packet& p = outQueue.front();

		try {
			uint32 len = p.length();
			socket->write(p.buffer, 2 + 4 + len);
			upBytesCounter += len + 6;
			outQueue.pop();
		} catch (const SocketException& se){
			if(se.code != SocketException::BUFFER_FULL) {
				throw se;
			}
			break;
		}
	}
}