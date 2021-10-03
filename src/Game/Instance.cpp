#include "Instance.h"
#include "Game/BlockInfo.h"
#include <iostream>

Instance::Instance(){
}
Instance::~Instance(){}

void Instance::registerBlock(BlockInfo bi){
	int size = bi.id - registeredBlocks.size() + 1;
	for(int i = 0; i < size; i++){
		registeredBlocks.emplace_back(BlockInfo());
	}
	registeredBlocks[bi.id] = bi;
}
