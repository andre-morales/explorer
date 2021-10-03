#include "Packet.h"
#include "ilib/Net/Socket.h"

Packet::Packet(){}
Packet::~Packet(){}

void Packet::op(PacketCode pc){
	this->opcode = (unsigned short)pc;
}

void Packet::send(Socket* s) const {
    send((Stream*)s);
}
void Packet::send(Stream* s) const {
	s->write((byte*)&opcode, 2);
	s->write((byte*)&length, 4);
	s->write(data, length);
}
std::string Packet::asString() const {
	return std::string((char*)data, length);
}
