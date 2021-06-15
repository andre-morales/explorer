#include "BlockInfo.h"

BlockInfo::BlockInfo(){}
BlockInfo::BlockInfo(std::string name, byte id, float opaqueness, byte texId)
: BlockInfo(name, id, opaqueness, texId, texId, texId){}


BlockInfo::BlockInfo(std::string name, byte id, float opaqueness, byte topId, byte sidesId, byte botId){
    this->name = name;
	this->id = id;
	this->opaqueness = opaqueness;
	this->opaque = opaqueness > 0.996;
	this->topTexId = topId;
	this->sidesTexId = sidesId;
	this->bottomTexId = botId;
}

