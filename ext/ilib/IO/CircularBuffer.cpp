#include "CircularBuffer.h"
#include <cstring>
#include "ilib/Math/maths.h"
#include "Exception.h"

CircularBuffer::CircularBuffer() : CircularBuffer(512){}

CircularBuffer::CircularBuffer(uint32 size)
: bufferSize(size), buffer(new byte[size]){
}

CircularBuffer::~CircularBuffer(){
	if(buffer) delete[] buffer;
}

uint32 CircularBuffer::write(const byte* ptr, uint32 length){
	if(full || !length) return 0;

	if(writeHead >= readHead) {
		uint32 frontAvailable = bufferSize - writeHead;
		if(frontAvailable >= length) {
			// Everything fits.
			if(ptr) memcpy(buffer + writeHead, ptr, length);
			writeHead += length;
			if(writeHead == bufferSize){ writeHead = 0; }
			if(writeHead == readHead){ full = true; }
			validateHeads();
			return length;
		} else {
			// Not everything fits. Write what we can.
			uint32 written = bufferSize - writeHead;
			if(ptr)  memcpy(buffer + writeHead, ptr, written);
			writeHead = 0;
			if(writeHead == readHead){
				full = true;
				return written;
			}

			// Write head is back at the beginning, run again.
			written += write(ptr + written, length - written);
			validateHeads();
			return written;
		}
	} else {
		uint32 frontAvailable = readHead - writeHead;
		uint32 towrite = Math::min(length, frontAvailable);

		if(ptr) memcpy(buffer + writeHead, ptr, towrite);
		writeHead += towrite;
		if(writeHead == readHead){ full = true; }
		validateHeads();
		return towrite;
	}
}

uint32 CircularBuffer::read(byte* ptr, uint32 len){
	if(!len) return 0;

	if(writeHead > readHead) {	
		uint32 avail = writeHead - readHead;
		uint32 toread = Math::min(len, avail);
		if(!avail) return 0;

		if(ptr) memcpy(ptr, buffer + readHead, toread);
		readHead += toread;
		validateHeads();
		return toread;
	} else {
		if(!full && readHead == writeHead) return 0;

		uint32 avail = bufferSize - readHead; // Available at the front of the read head. 
		full = false;

		if(avail >= len) {
			if(ptr) memcpy(ptr, buffer + readHead, len);
			readHead += len;
			if(readHead == bufferSize) { readHead = 0; }
			validateHeads();
			return len;
		} else {
			if(ptr) memcpy(ptr, buffer + readHead, avail);
			readHead = 0;
			validateHeads();
			return avail + read(ptr + avail, len - avail);
		}
	}
}

uint32 CircularBuffer::toRead(){
	if(!full) {
		if(writeHead >= readHead) {
			return writeHead - readHead;
		} else {
			return bufferSize + writeHead - readHead;
		}
	}	
	return bufferSize;
}
uint32 CircularBuffer::toWrite(){
	if(!full) {
		if(writeHead >= readHead) {
			return bufferSize - writeHead + readHead;
		} else {
			return readHead - writeHead;
		}
	}	
	return 0;
}

void CircularBuffer::fixWriteHead() {
	validateHeads();

	if(writeHead == bufferSize) {
		writeHead = 0;	
	}
	if(writeHead == readHead) full = true;
}

void CircularBuffer::fixReadHead() {
	validateHeads();

	if(readHead == bufferSize) {
		readHead = 0;
	}
	full = false;
}

uint32 CircularBuffer::toWrite_infront(){
	if(full) return 0;

	if(writeHead >= readHead) {
		return bufferSize - writeHead;
	} else {
		return readHead - writeHead;
	}
}
uint32 CircularBuffer::toRead_infront(){
	if(full) return bufferSize - readHead;

	if(writeHead >= readHead) {
		return writeHead - readHead;
	} else {
		return bufferSize - readHead;
	}	
}
void CircularBuffer::validateHeads () {
	if(writeHead > bufferSize) throw Exception("invalid write head.");
	if(readHead > bufferSize) throw Exception("invalid read head.");
}