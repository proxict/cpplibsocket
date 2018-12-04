#include "cpplibsocket/Socket.h"

namespace cpplibsocket {

Socket<IPProto::TCP>::Socket(const IPVer ipVersion)
    : SocketBase(IPProto::TCP, ipVersion) {}

Socket<IPProto::TCP>::Socket(const IPVer ipVersion, const SocketHandle clientFileDescriptor)
    : SocketBase(IPProto::TCP, ipVersion, clientFileDescriptor) {}

void Socket<IPProto::TCP>::connect(const std::string& hostIp, const Port hostPort) {
    const sockaddr addr = createAddr(hostIp, hostPort);
    if (::connect(mSocketHandle, &addr, getAddrSize(mIpVersion)) == -1) {
        throw Exception(
            FUNC_NAME, "Couldn't connect to \"", hostIp, ":", hostPort, "\" - ", getLastErrorFormatted());
    }
}

void Socket<IPProto::TCP>::listen(const int backLogSize) {
    if (::listen(mSocketHandle, backLogSize) == -1) {
        throw Exception(FUNC_NAME, "Couldn't open socket for listening - ", getLastErrorFormatted());
    }
}

Expected<Socket<IPProto::TCP>> Socket<IPProto::TCP>::accept() const {
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

Expected<SentSize> Socket<IPProto::TCP>::send(const Byte* data, const DataSize size) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    const SentSize sent = Platform::send(mSocketHandle, data, size);
    if (sent == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't send data - ", getLastErrorFormatted());
    }
    return sent;
}

Expected<ReceivedSize> Socket<IPProto::TCP>::receive(Byte* data, const DataSize maxSize) const {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Couldn't receive data");
    }
    const ReceivedSize received = Platform::receive(mSocketHandle, data, maxSize);
    if (received == -1) {
        if (errno == EWOULDBLOCK) {
            return makeUnexpected<Size>("Socket would block");
        }
        throw Exception(FUNC_NAME, "Couldn't receive data - ", getLastErrorFormatted());
    }
    return received;
}

} // namespace cpplibsocket
