#include "Packet.h"
#include "ilib/Net/Socket.h"

Packet::Packet(){}
Packet::~Packet(){}

void Packet::send(Stream& s){
	s.write((byte*)&opcode, 2);
	s.write((byte*)&length, 4);
	s.write(data, length);
}

void Packet::get(Stream& s){
	s.read((byte*)&opcode, 2);
	s.read((byte*)&length, 4);
	data = new byte[length];
	s.read((byte*)data, length);
}
