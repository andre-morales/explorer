#pragma once
#include "ilib/mem.h"
#include <unordered_set>
#include <mutex>

struct BatchRequest {
	unsigned char code;
	class Chunk* chunk;
};

struct GameImpl{
	class Game& game;
	un<class Thread> batchingThread;
	std::unordered_set<BatchRequest*> chunkBatchingQueue;
	std::condition_variable chunkAvailableCV;
	std::mutex chunkAvailableLock;

	GameImpl(Game& g);
	~GameImpl();
};