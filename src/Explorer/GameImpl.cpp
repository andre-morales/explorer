#include "GameImpl.h"
#include "Game.h"
#include "ilib/IO/Compression/ZL_DecompressorStream.h"
#include "ilib/IO/Compression/BZ2_DecompressorStream.h"
#include "ilib/IO/CircularBuffer.h"
#include "ilib/Net/Socket.h"
#include "ilib/Net/SocketException.h"
#include "ilib/thread.h"

GameImpl::GameImpl(Game& g) : This(g),
	inputStream(new BZ2_DecompressorStream(512 * 64, 1024 * 32)){
};
GameImpl::~GameImpl(){};

void GameImpl::processOutQueue () {
	if(!socket) return;
	while(!outQueue.empty ()) {
		const Packet& p = outQueue.front();

		try {
			uint32 len = p.length();
			socket->write(p.buffer, 1 + 4 + len);
			upBytesCounter += len + 5;
			outQueue.pop();
		} catch (const SocketException& se){
			if(se.code != SocketException::BUFFER_FULL) {
				throw se;
			}
			break;
		}
	}
}