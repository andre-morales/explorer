#include "Packet.h"
#include "ilib/Net/Socket.h"
#include "Exception.h"

Packet::Packet(){}

Packet::Packet(byte* b, Flags f) : buffer(b), flags(f){}

Packet::Packet(Packet&& p) noexcept : buffer(p.buffer), flags(p.flags) {}

Packet::~Packet(){
	if(flags.unique_buffer){
		delete[] buffer;
	}
}

void Packet::create(uint8 op, uint32 len) {
	if(buffer) throw Exception("Packet already has a buffer.");
	buffer = new byte[5 + len];
	opcode(op);
	length(len);
}

void Packet::create(uint8 op, const byte* ptr, uint32 len) {
	if(buffer) throw Exception("Packet already has a buffer.");
	buffer = new byte[5 + len];
	opcode(op);
	length(len);
	data_copy(ptr);
}

uint8 Packet::opcode() const {
	return *(uint8*)buffer;
}

void Packet::opcode(uint8 op) {
	*(uint8*)buffer = op;
}

uint32 Packet::length() const {
	return *(uint32*)(buffer + 1);
}

void Packet::length(uint32 len) {
	*(uint32*)(buffer + 1) = len;
}

byte* Packet::data() const {
	return buffer + 5;
}

void Packet::data_copy(const byte* newdata){
	uintptr len = length();
	memcpy(buffer + 5, newdata, len);
}

void Packet::data_copy_len(const byte* newdata, uint32 len){
	length(len);
	memcpy(buffer + 5, newdata, len);
}

void Packet::destroy () {
	delete[] buffer;
	buffer = nullptr;
}

void Packet::release(){
	buffer = nullptr;
}

//void Packet::send(Stream* str){
	//uint32 size = 5 + length();
	//if(str->write(buffer, size) != size) throw Exception("Illegal send!");
//}