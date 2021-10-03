#pragma once

enum class PacketCode {
    EMPTY = 0,
	HANDSHAKE = 1,
	JOIN = 2,
	CHUNK = 3,
	CHAT_MSG = 4
};
