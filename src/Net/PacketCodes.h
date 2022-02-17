#pragma once
#include "ilib/types.h"

namespace PacketInfo {
	typedef const uint8 Code;

	// Gereral connection packets, these can be sent at any given time.
	Code EMPTY = 0; // Does nothing.

	// Signals a general error.
	Code ERROR = 1; 
	namespace Error {
		Code INVALID_STATE         = 0;
		Code UNSUPPORTED_OPERATION = 1;
		Code ILLEGAL_PARAMETERS    = 2;
	}

	// Negotiates connection settings
	Code SET_CONNECTION = 2;
	namespace SetConnection {
		Code APPLY_CONNECTION_SETTINGS = 0;
		Code LIST_CLIENT_IN_COMP_WAYS  = 1;
		Code SET_CLIENT_IN_COMP_WAY    = 2;
	}
	Code DISCONNECT    = 3; // Server: order to disconnect | Client: quit from server. 
	
	// Game mechanics packets.
	Code JOIN		   = 4; // Client: sent to the server when joining.
	Code STRUCTURE	   = 5; // Server: indicates allocation of universes/planets.
	Code CHUNK		   = 6; // Client: means a chunk data request, Server: means a chunk delivery.
	Code CHAT_MSG	   = 7; // Client: sent chat message, Server: chat text.
	Code TELEPORT	   = 8; // Server: instruction to either teleport or spawn the player at some other position.
};
