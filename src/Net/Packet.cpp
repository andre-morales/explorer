#include "Packet.h"
#include "ilib/Net/Socket.h"
#include "Exception.h"

Packet::Packet(){}

Packet::Packet(uint32 l, byte* d, uint16 o, byte f)
: length(l), data(d), opcode(o){}

Packet::Packet(Packet&& p) noexcept {
	length = p.length;
	data = p.data;
	opcode = p.opcode;
	flags = p.flags;
}

Packet::~Packet(){
	if(flags == 1){
		delete[] data;
	}
}

void Packet::copyFullData(const void* src, uint32 len){
	if(data) throw Exception("Packet already contains data, unspecified behaviour!");
	length = len;
	data = new byte[len];
	memcpy(data, src, len);
}

void Packet::setDeleteFlag(){
	flags = 1;
}

void Packet::send(Socket* s) const {
    send((Stream*)s);
}
void Packet::send(Stream* s) const {
	s->write((byte*)&opcode, 2);
	s->write((byte*)&length, 4);
	if(data) s->write(data, length);
}
std::string Packet::asString() const {
	return std::string((char*)data, length);
}
