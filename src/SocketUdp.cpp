#include "cpplibsocket/Socket.h"

namespace cpplibsocket {

Socket<IPProto::UDP>::Socket(const IPVer ipVersion)
    : SocketBase(IPProto::UDP, ipVersion) {}

Expected<UnsignedSize> Socket<IPProto::UDP>::sendTo(const Byte* data,
                                                const UnsignedSize size,
                                                const std::string& hostIp,
                                                const Port hostPort) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't send data");
    }
    const sockaddr addr = createAddr(hostIp, hostPort);
    const SignedSize sent = Platform::sendTo(mSocketHandle, data, size, &addr);
    if (sent == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    ASSERT(sent >= 0);
    return static_cast<UnsignedSize>(sent);
}

Expected<UnsignedSize> Socket<IPProto::UDP>::receiveFrom(Byte* data,
                                                         const UnsignedSize maxSize,
                                                         IPVer& sourceIpVersion,
                                                         std::string& sourceIp,
                                                         Port& sourcePort) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    sockaddr addr;
    const SignedSize received = Platform::receiveFrom(mSocketHandle, data, maxSize, &addr);
    if (received == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    // TODO: Function toIpVersion(int nativeIpVersion);
    sourceIpVersion = addr.sa_family == AF_INET ? IPVer::IPV4 : IPVer::IPV6;
    switch (addr.sa_family) {
    case AF_INET: {
        char addrRaw[16] = {};
        ::inet_ntop(addr.sa_family, reinterpret_cast<const sockaddr_in*>(&addr), addrRaw, sizeof(addrRaw));
        sourceIp = addrRaw;
        sourcePort = ntohs(reinterpret_cast<const sockaddr_in*>(&addr)->sin_port);
        break;
    }
    case AF_INET6: {
        char addrRaw[40] = {};
        ::inet_ntop(addr.sa_family, reinterpret_cast<const sockaddr_in6*>(&addr), addrRaw, sizeof(addrRaw));
        sourceIp = addrRaw;
        sourcePort = ntohs(reinterpret_cast<const sockaddr_in6*>(&addr)->sin6_port);
        break;
    }
    default:
        throw Exception(FUNC_NAME, "Unknown IP version from client");
    }
    ASSERT(received >= 0);
    return static_cast<UnsignedSize>(received);
}

} // namespace cpplibsocket
