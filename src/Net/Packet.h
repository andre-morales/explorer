#pragma once
#include "ilib/types.h"
#include "Net/PacketCodes.h"
#include <string>

class Socket;
class Stream;

class Packet {
public:
	byte* buffer = 0;
	struct Flags {
		/* When set, this packet owns its data pointer, and shall delete its data when the packet itself gets destroyed. */
		bool unique_buffer : 1;

		bool stack_allocated : 1;
	} flags = {false, false};

	Packet();
	Packet(byte*, Flags);
	Packet(Packet&&) noexcept;
	~Packet();

	uint8 opcode() const;
	void opcode(uint8);

	uint32 length() const;
	void length(uint32);

	/* Pointer to data section of the packet. Equivalent to buffer() + 6 */
	byte* data() const;

	/* Copies 'length' bytes from ptr. Remember to use length() before calling this. */
	void data_copy(const byte* ptr);

	/* Copies len bytes from ptr. Equivalent to  
		length(len)
		data_copy(ptr)*/
	void data_copy_len(const byte* ptr, uint32 len);

	/* Allocates a new buffer of size len + 6. Equivalent to
	.buffer = new byte[6 + len]
	opcode(op)
	length(len) */
	void create(uint8 op, uint32 len);

	/* Allocates a new buffer and copies the data in ptr. Equivalent to
		.buffer = new byte[6 + len]
		opcode(op)
		length(len)
		data_copy(ptr) */
	void create(uint8 op, const byte* ptr, uint32 len);

	/* Call delete[] on buffer pointer and sets it to nullptr. */
	void destroy();

	/* Sets buffer ptr to nullptr. */
	void release();

	//void send(Stream*);
private:
	Packet(const Packet&) = delete;
};
