#pragma once
#include "Net/Packet.h"
#include <thread>
#include <string>
#include <queue>
#include <mutex>
#include "ilib/mem.h"

class Client {
public:
	class Socket* socket = 0;
	std::thread* thread = 0;
	std::string compressionIn = "raw";
	std::string compressionOut = "raw";
	std::string name;
	bool joined = false;
	bool disconnected = false;

	std::queue<sh<Packet>> outQueue;
	std::mutex outQueueLock;
	un<class Stream> outBuffer;


	void enqueue_sync(const sh<Packet>&);
};

