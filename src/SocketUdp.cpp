#include "cpplibsocket/Socket.h"
#include "cpplibsocket/utils/utils.h"

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
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    ASSERT(sent >= 0);
    return static_cast<UnsignedSize>(sent);
}

Expected<UnsignedSize> Socket<IPProto::UDP>::receiveFrom(Byte* data, const UnsignedSize maxSize) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    sockaddr addr;
    const SignedSize received = Platform::receiveFrom(mSocketHandle, data, maxSize, &addr);
    if (received == -1) {
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    ASSERT(received >= 0);
    return static_cast<UnsignedSize>(received);
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
        if (errno == EWOULDBLOCK || errno == EAGAIN) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    sourceIpVersion = toIPVer(addr.sa_family);
    sourceIp = utils::detail::getIpAddress(addr);
    sourcePort = utils::detail::getPort(addr);

    ASSERT(received >= 0);
    return static_cast<UnsignedSize>(received);
}

} // namespace cpplibsocket
