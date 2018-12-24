#include "cpplibsocket/Socket.h"

namespace cpplibsocket {

Socket<IPProto::TCP>::Socket(const IPVer ipVersion)
    : SocketBase(IPProto::TCP, ipVersion) {}

void Socket<IPProto::TCP>::connect(const std::string& hostIp, const Port hostPort) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    const sockaddr addr = createAddr(hostIp, hostPort);
    if (::connect(mSocketHandle, &addr, getAddrSize(mIpVersion)) == -1) {
        throw Exception(
            FUNC_NAME, "Couldn't connect to \"", hostIp, ":", hostPort, "\" - ", getLastErrorFormatted());
    }
}

void Socket<IPProto::TCP>::listen(const int backLogSize) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    if (::listen(mSocketHandle, backLogSize) == -1) {
        throw Exception(FUNC_NAME, "Couldn't open socket for listening - ", getLastErrorFormatted());
    }
}

Expected<Socket<IPProto::TCP>> Socket<IPProto::TCP>::accept() const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    sockaddr sourceAddr = {};
    SockLenType addrLen = sizeof(sockaddr);
    const SocketHandle clientFileDescriptor = ::accept(mSocketHandle, &sourceAddr, &addrLen);
    if (clientFileDescriptor == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Socket>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't accept client - ", getLastErrorFormatted());
    }
    return Socket<IPProto::TCP>(sourceAddr.sa_family == AF_INET ? IPVer::IPV4 : IPVer::IPV6,
                                clientFileDescriptor);
}

Expected<UnsignedSize> Socket<IPProto::TCP>::send(const Byte* data, const UnsignedSize size) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    const SignedSize sent = Platform::send(mSocketHandle, data, size);
    if (sent == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    ASSERT(sent >= 0);
    return static_cast<UnsignedSize>(sent);
}

Expected<UnsignedSize> Socket<IPProto::TCP>::receive(Byte* data, const UnsignedSize maxSize) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    const SignedSize received = Platform::receive(mSocketHandle, data, maxSize);
    if (received == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    ASSERT(received >= 0);
    return static_cast<UnsignedSize>(received);
}

Socket<IPProto::TCP>::Socket(const IPVer ipVersion, const SocketHandle clientSocketHandle) noexcept
    : SocketBase(IPProto::TCP, ipVersion, clientSocketHandle) {}

} // namespace cpplibsocket
