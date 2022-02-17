#include "ZL_DecompressorStream.h"
#include "ilib/IO/Compression/ZL_Decompressor.h"

ZL_DecompressorStream::ZL_DecompressorStream(uint32 is, uint32 os) :
	inBuffer(new CircularBuffer(is)), 
	outBuffer(new CircularBuffer(os)),
	inflate_(new ZL_Decompressor()){}

ZL_DecompressorStream::~ZL_DecompressorStream(){
	delete inflate_;
}

uint32 ZL_DecompressorStream::write(const byte* data, uint32 len){
	uint32 writtenData = inBuffer->write(data, len);

	byte* inPtr = inBuffer->buffer + inBuffer->readHead;
	uint32 availIn = inBuffer->toRead_infront();
	uint32 read;

	byte* outPtr = outBuffer->buffer + outBuffer->writeHead;
	uint32 availOut = outBuffer->toWrite_infront();
	uint32 written;

	inflate_->decompress(inPtr, availIn, read, outPtr, availOut, written);

	if(read){
		inBuffer->readHead += read;
		inBuffer->fixReadHead();
	}

	if(written) {
		outBuffer->writeHead += written;
		outBuffer->fixWriteHead();
	}

	return writtenData;
}

uint32 ZL_DecompressorStream::writeFrom(Stream* str, uint32 len) {
	byte* ptr = inBuffer->buffer + inBuffer->writeHead;
	uint32 avail_space = inBuffer->toWrite_infront();

	uint32 written = str->read(ptr, std::min(avail_space, len));

	if(written) {
		inBuffer->writeHead += written;
		inBuffer->fixWriteHead();
	}

	return written;
}

uint32 ZL_DecompressorStream::read(byte* data, uint32 len){
	return outBuffer->read(data, len);
}



uint32 ZL_DecompressorStream::toWrite(){
	return inBuffer->toWrite();
}

uint32 ZL_DecompressorStream::toRead(){
	return outBuffer->toRead();
}