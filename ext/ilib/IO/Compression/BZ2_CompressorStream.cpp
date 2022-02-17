#include "BZ2_CompressorStream.h"
#include "ilib/IO/Compression/BZ2_Compressor.h"

BZ2_CompressorStream::BZ2_CompressorStream(uint32 is, uint32 os) :
inBuffer(new CircularBuffer(is)), 
outBuffer(new CircularBuffer(os)),
deflate_(new BZ2_Compressor()){}

BZ2_CompressorStream::~BZ2_CompressorStream(){
	delete deflate_;
}

uint32 BZ2_CompressorStream::write(const byte* data, uint32 len){
	uint32 writtenData = inBuffer->write(data, len);

	byte* inPtr = inBuffer->buffer + inBuffer->readHead;
	uint32 availIn = inBuffer->toRead_infront();
	uint32 read = 0;

	uint32 rh = outBuffer->readHead;
	uint32 wh = outBuffer->writeHead;
	
	byte* outPtr = outBuffer->buffer + wh;
	uint32 availOut = outBuffer->toWrite_infront();
	uint32 written = 0;

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

uint32 BZ2_CompressorStream::read(byte* data, uint32 len){
	return outBuffer->read(data, len);
}

uint32 BZ2_CompressorStream::readInto(Stream* stream, uint32 len){
	return outBuffer->readInto(stream, len);
}

uint32 BZ2_CompressorStream::toWrite(){
	return inBuffer->toWrite();
}

void BZ2_CompressorStream::flush_sync () {
	do {	
		write(nullptr, 0);

		uint32 written;
		do {
			byte* outPtr = outBuffer->buffer + outBuffer->writeHead;
			uint32 availOut = outBuffer->toWrite_infront();
			
			deflate_->flush_sync(outPtr, availOut, written);

			if(written){
				outBuffer->writeHead += written;
				outBuffer->fixWriteHead();
			}
		} while(written);
	} while(inBuffer->toRead() > 0 && outBuffer->toWrite() > 0);


}