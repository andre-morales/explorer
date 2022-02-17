#include "ZL_CompressorStream.h"
#include "ilib/IO/Compression/ZL_Compressor.h"

ZL_CompressorStream::ZL_CompressorStream(uint32 is, uint32 os) :
inBuffer(new CircularBuffer(is)), 
outBuffer(new CircularBuffer(os)),
deflate_(new ZL_Compressor()){}

ZL_CompressorStream::~ZL_CompressorStream(){
	delete deflate_;
}

uint32 ZL_CompressorStream::write(const byte* data, uint32 len){
	uint32 writtenData = inBuffer->write(data, len);

	byte* inPtr = inBuffer->buffer + inBuffer->readHead;
	uint32 availIn = inBuffer->toRead_infront();
	uint32 read;

	uint32 rh = outBuffer->readHead;
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

uint32 ZL_CompressorStream::readInto(Stream* stream, uint32 len){
	return outBuffer->readInto(stream, len);
}

uint32 ZL_CompressorStream::read(byte* data, uint32 len){
	return outBuffer->read(data, len);
}

uint32 ZL_CompressorStream::toWrite(){
	return inBuffer->toWrite();
}

uint32 ZL_CompressorStream::toRead(){
	return outBuffer->toRead();
}


void ZL_CompressorStream::flush_sync () {
	uint32 input = inBuffer->toRead();
	uint32 written = 0;

	while(true){	
		write(nullptr, 0);
		uint32 input_ = inBuffer->toRead();

		byte* outPtr = outBuffer->buffer + outBuffer->writeHead;
		uint32 availOut = outBuffer->toWrite_infront();
		uint32 written_;

		deflate_->flush_sync(outPtr, availOut, written_);

		if(written_){
			outBuffer->writeHead += written_;
			outBuffer->fixWriteHead();
		}

		if(input_ == input && written == written_) break;
		input = input_;
		written = written_;
	}
}