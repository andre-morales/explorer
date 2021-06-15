#pragma once

enum class PacketCode {
	HANDSHAKE = 1,
	JOIN = 2,
	CHUNK_REQUEST = 3,
	CHAT_MSG = 4
};
