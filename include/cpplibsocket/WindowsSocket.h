#include "cpplibsocket/SocketCommon.h"

namespace cpplibsocket {

std::string getErrorString(const DWORD errorId);

std::string getLastErrorFormatted();

bool winsockInitialized();

bool initializeWinsock();

void deinitializeWinsock();

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size);

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr);

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize size);

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize size, sockaddr* addr);

    bool setBlocked(SocketHandle socket, const bool blocked = true);

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion);

    bool closeSocket(SocketHandle socket);

    static constexpr SocketHandle SOCKET_NULL = INVALID_SOCKET;

} // namespace Platform

} // namespace cpplibsocket

