#include "DeflateStream.h"
#include "ilib/IO/Compression/Deflate.h"

DeflateStream::DeflateStream(uint32 is, uint32 os) :
inBuffer(new CircularBuffer(is)), 
outBuffer(new CircularBuffer(os)),
deflate_(new Deflate()){}

DeflateStream::~DeflateStream(){
	delete deflate_;
}

uint32 DeflateStream::write(const byte* data, uint32 len){
	uint32 writtenData = inBuffer->write(data, len);

	
	byte* inPtr = inBuffer->buffer + inBuffer->readHead;
	uint32 availIn = inBuffer->toRead_infront();
	uint32 read;

	uint32 rh =  outBuffer->readHead;
	uint32 wh = outBuffer->writeHead;
	
	byte* outPtr = outBuffer->buffer + wh;
	uint32 availOut = outBuffer->toWrite_infront();
	uint32 written;

	deflate_->compress(inPtr, availIn, read, outPtr, availOut, written);

	if(read){
		inBuffer->readHead += read;
		inBuffer->fixReadHead();
	}

	if(written){
		outBuffer->writeHead += written;
		outBuffer->fixWriteHead();
	}

	return writtenData;
}

uint32 DeflateStream::read(byte* data, uint32 len){
	return outBuffer->read(data, len);
}

uint32 DeflateStream::toWrite(){
	return inBuffer->toWrite();
}