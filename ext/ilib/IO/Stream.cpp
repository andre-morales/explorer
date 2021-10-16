#include "Stream.h"
#include "Exception.h"

Stream::Stream(){}
Stream::~Stream(){}
uint32 Stream::write(const byte*, uint32){
	throw Exception("Not implemented.");
}
uint32 Stream::read(byte*, uint32){
	throw Exception("Not implemented.");
}
uint32 Stream::toRead(){
	throw Exception("Not implemented.");
}
uint32 Stream::toWrite(){
	throw Exception("Not implemented.");
}
uint32 Stream::readInto(Stream*, uint32){
	throw Exception("Not implemented.");
}
uint32 Stream::writeFrom(Stream*, uint32){
	throw Exception("Not implemented.");
}