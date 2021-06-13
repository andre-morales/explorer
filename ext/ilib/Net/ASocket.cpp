#include "ASocket.h"
#include "Socket.h"
#include <iostream>
#include "SocketException.h"

void ASocket::spinReadEx(Socket& s, byte* buffer, uint32 len){
    while(len > 0){
        int res = s.read(buffer, len);
        if(res == -1) throw SocketException("Read failed.");
        if(res > 0) len -= res;
    }
}
