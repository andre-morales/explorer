#include "BZ2_DecompressorStream.h"
#include "ilib/IO/Compression/BZ2_Decompressor.h"

BZ2_DecompressorStream::BZ2_DecompressorStream(uint32 is, uint32 os) :
	inBuffer(new CircularBuffer(is)), 
	outBuffer(new CircularBuffer(os)),
	inflate_(new BZ2_Decompressor()){}

BZ2_DecompressorStream::~BZ2_DecompressorStream(){
	delete inflate_;
}

uint32 BZ2_DecompressorStream::write(const byte* data, uint32 len){
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

uint32 BZ2_DecompressorStream::writeFrom(Stream* str, uint32 len) {
	byte* ptr = inBuffer->buffer + inBuffer->writeHead;
	uint32 avail_space = inBuffer->toWrite_infront();

	uint32 written = str->read(ptr, std::min(avail_space, len));

	if(written) {
		inBuffer->writeHead += written;
		inBuffer->fixWriteHead();
	}

	return written;
}

uint32 BZ2_DecompressorStream::read(byte* data, uint32 len){
	return outBuffer->read(data, len);
}



uint32 BZ2_DecompressorStream::toWrite(){
	return inBuffer->toWrite();
}

uint32 BZ2_DecompressorStream::toRead(){
	return outBuffer->toRead();
}