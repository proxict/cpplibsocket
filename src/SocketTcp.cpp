#include "cpplibsocket/SocketTcp.h"
#include "cpplibsocket/utils/EndpointPrint.h"
#include "cpplibsocket/utils/utils.h"

#include <sstream>

namespace cpplibsocket {

Socket<IPProto::TCP>::Socket(const IPVer ipVersion)
    : SocketBase(IPProto::TCP, ipVersion) {}

void Socket<IPProto::TCP>::connect(const Address& address) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    if (::connect(mSocketHandle, &address.sa, getAddrSize(mIpVersion)) == -1) {
        std::ostringstream ss;
        ss << "Couldn't connect to " << utils::getEndpoint(address) << " - " << getLastErrorFormatted();
        throw Exception(FUNC_NAME, ss.str());
    }
}

void Socket<IPProto::TCP>::connect(const std::string& hostIp, const Port hostPort) {
    connect(createAddr(hostIp, hostPort));
}

void Socket<IPProto::TCP>::listen(const int backlogSize) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    if (::listen(mSocketHandle, backlogSize) == -1) {
        throw Exception(FUNC_NAME, "Couldn't open socket for listening - ", getLastErrorFormatted());
    }
}

Expected<Socket<IPProto::TCP>, WouldBlock> Socket<IPProto::TCP>::accept() const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "The socket is not open");
    }
    sockaddr_in6 addr6 = {};
    SockLenType addrLen = sizeof(sockaddr_in6);
    const SocketHandle clientFileDescriptor =
        ::accept(mSocketHandle, reinterpret_cast<sockaddr*>(&addr6), &addrLen);
    if (clientFileDescriptor == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected(WouldBlock{});
        }
        throw Exception(FUNC_NAME, "Couldn't accept client - ", getLastErrorFormatted());
    }
    return Socket<IPProto::TCP>(mIpVersion, clientFileDescriptor);
}

Expected<UnsignedSize, WouldBlock> Socket<IPProto::TCP>::send(const Byte* data,
                                                              const UnsignedSize size) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    const SignedSize sent = Platform::send(mSocketHandle, data, size);
    if (sent == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected(WouldBlock{});
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    ASSERT(sent >= 0);
    return static_cast<UnsignedSize>(sent);
}

Expected<UnsignedSize, WouldBlock> Socket<IPProto::TCP>::receive(Byte* data,
                                                                 const UnsignedSize maxSize) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    const SignedSize received = Platform::receive(mSocketHandle, data, maxSize);
    if (received == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected(WouldBlock{});
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    ASSERT(received >= 0);
    return static_cast<UnsignedSize>(received);
}

Socket<IPProto::TCP>::Socket(const IPVer ipVersion, const SocketHandle clientSocketHandle) noexcept
    : SocketBase(IPProto::TCP, ipVersion, clientSocketHandle) {}

} // namespace cpplibsocket
