#include "Instance.h"
#include "Game/BlockInfo.h"

Instance::Instance(){
    for(int i = 0; i < 255; i++){
        BlockInfo bi{"", i, true, 1, 1, 1, false};
        bi.id = i;
        registeredBlocks.emplace_back(bi);
    }
}
Instance::~Instance(){}

void Instance::registerBlock(byte id){

}
