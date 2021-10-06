#include "Client.h"

void Client::enqueue_sync(const sh<Packet>& packet){
	outQueueLock.lock();
	outQueue.emplace(packet);
	outQueueLock.unlock();
}