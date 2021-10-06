#pragma once

#include "Net/Packet.h"
#include <thread>
#include <string>
#include <set>
#include <mutex>
#include "ilib/mem.h"

class Client {
public:
	class Socket* socket;
	std::thread* thread = 0;
	std::string name;
	bool disconnected = false;
	std::set<sh<Packet>> outQueue;
	std::mutex outQueueLock;
	void enqueue_sync(const sh<Packet>&);
};

