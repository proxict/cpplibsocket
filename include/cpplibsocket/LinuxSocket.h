#ifndef CPPLIBSOCKET_LINUXSOCKET_H_
#define CPPLIBSOCKET_LINUXSOCKET_H_

#include "cpplibsocket/SocketCommon.h"

#include <fcntl.h>
#include <unistd.h>

namespace cpplibsocket {

/// Returns a string representation of the last error that occurred in either system calls or in some library
/// functions.
std::string getLastErrorFormatted();

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size);

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr);

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize size);

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize size, sockaddr* addr);

    bool setBlocked(SocketHandle socket, const bool blocked = true);

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion);

    int closeSocket(SocketHandle socket);

    static constexpr SocketHandle SOCKET_NULL = -1;

} // namespace Platform

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_LINUXSOCKET_H_
