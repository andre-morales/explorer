#pragma once
#include "ilib/mem.h"
#include <unordered_set>
#include <mutex>
#include <queue>
#include "Net/Packet.h"

struct BatchRequest {
	unsigned char code;
	class Chunk* chunk;
};

class GameImpl {
public:
	class Game& This;
	class Socket* socket = nullptr;
	un<class InflateStream> inflater;
	un<class Thread> batchingThread;
	std::unordered_set<BatchRequest*> chunkBatchingQueue;
	std::condition_variable chunkAvailableCV;
	std::mutex chunkAvailableLock;
	std::queue<Packet> outQueue;

	void* keyListener = 0;
	void* charListener = 0;
	void* mouseButtonListener = 0;
	void* mouseMotionListener = 0;

	uint64 downBytesCounter = 0;
	uint64 upBytesCounter = 0;

	byte readInHeader[6] = {0};

	GameImpl(Game& g);
	~GameImpl();

	void processOutQueue ();
};