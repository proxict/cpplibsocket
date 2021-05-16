#include "cpplibsocket/SocketUdp.h"
#include "cpplibsocket/utils/utils.h"

namespace cpplibsocket {

Socket<IPProto::UDP>::Socket(const IPVer ipVersion)
    : SocketBase(IPProto::UDP, ipVersion) {}

Expected<UnsignedSize, WouldBlock> Socket<IPProto::UDP>::sendTo(const Byte* data,
                                                                const UnsignedSize size,
                                                                const std::string& hostIp,
                                                                const Port hostPort) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't send data");
    }
    const sockaddr_storage addr = createAddr(hostIp, hostPort);
    const SignedSize sent =
        Platform::sendTo(mSocketHandle, data, size, reinterpret_cast<const sockaddr*>(&addr));
    if (sent == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected(WouldBlock{});
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    ASSERT(sent >= 0);
    return static_cast<UnsignedSize>(sent);
}

Expected<UnsignedSize, WouldBlock>
Socket<IPProto::UDP>::receiveFrom(Byte* data, const UnsignedSize maxSize, Endpoint* source) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    sockaddr_storage addr;
    const SignedSize received =
        Platform::receiveFrom(mSocketHandle, data, maxSize, reinterpret_cast<sockaddr*>(&addr));
    if (received == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected(WouldBlock{});
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    ASSERT(received >= 0);
    *source = utils::getEndpoint(reinterpret_cast<sockaddr*>(&addr));
    return static_cast<UnsignedSize>(received);
}

} // namespace cpplibsocket
