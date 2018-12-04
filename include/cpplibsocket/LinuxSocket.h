#ifndef CPPLIBSOCKET_LINUXSOCKET_H_
#define CPPLIBSOCKET_LINUXSOCKET_H_

#include "cpplibsocket/SocketCommon.h"

#include <fcntl.h>
#include <unistd.h>

namespace cpplibsocket {

std::string getLastErrorFormatted();

namespace Platform {

    DataSize send(SocketHandle socket, const Byte* data, const DataSize size);

    DataSize sendTo(SocketHandle socket, const Byte* data, const DataSize size, const sockaddr* addr);

    DataSize receive(SocketHandle socket, Byte* data, const DataSize size);

    DataSize receiveFrom(SocketHandle socket, Byte* data, const DataSize size, sockaddr* addr);

    bool setBlocked(SocketHandle socket, const bool blocked = true);

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion);

    int closeSocket(SocketHandle socket);

    static constexpr SocketHandle SOCKET_NULL = -1;

} // namespace Platform


} // namespace cpplibsocket

#endif // CPPLIBSOCKET_LINUXSOCKET_H_
