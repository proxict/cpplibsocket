#include "cpplibsocket/Socket.h"

namespace cpplibsocket {

SocketBase::~SocketBase() {
    try {
        if (isOpen()) {
            close();
        }
    } catch (...) {
    }
}

SocketBase& SocketBase::operator=(SocketBase&& other) {
    mSocketHandle = other.mSocketHandle;
    mIpVersion = other.mIpVersion;
    other.mSocketHandle =
        Platform::SOCKET_NULL; // So the destructor of the moved object doesn't close our socket
    return *this;
}

SocketBase::SocketBase(SocketBase&& other) {
    *this = std::move(other);
}

bool SocketBase::isOpen() const noexcept {
    return mSocketHandle != Platform::SOCKET_NULL;
}

void SocketBase::open() {
    if (isOpen()) {
        throw Exception(FUNC_NAME, "Socket is already open");
    }

    mSocketHandle = Platform::openSocket(mIpProtocol, mIpVersion);
    if (mSocketHandle == Platform::SOCKET_NULL) {
        throw Exception(FUNC_NAME, "Couldn't open socket - ", getLastErrorFormatted());
    }
}

void SocketBase::close() {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    if (!Platform::closeSocket(mSocketHandle)) {
        throw Exception(FUNC_NAME, "Couldn't close socket - ", getLastErrorFormatted());
    }
    mSocketHandle = Platform::SOCKET_NULL;
}

Port SocketBase::bind(const std::string& ip, const Port port) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    sockaddr addr = createAddr(ip, port);
    if (::bind(mSocketHandle, &addr, getAddrSize(mIpVersion)) != 0) {
        throw Exception(
            FUNC_NAME, "Couldn't bind address \"", ip, ":", port, "\" - ", getLastErrorFormatted());
    }

    sockaddr_in sin;
    SockLenType len = sizeof(sin);
    if (::getsockname(mSocketHandle, reinterpret_cast<sockaddr*>(&sin), &len) != 0) {
        throw Exception(FUNC_NAME, "Couldn't get the bound port - ", getLastErrorFormatted());
    }
    return ntohs(sin.sin_port);
}

void SocketBase::setBlocked(const bool blocked) {
    if (!isOpen()) {
        throw Exception(FUNC_NAME, "Socket is not open");
    }
    if (!Platform::setBlocked(mSocketHandle, blocked)) {
        throw Exception(
            FUNC_NAME, "Couldn't set blocking/non-blocking socket property - ", getLastErrorFormatted());
    }
}

SocketBase::SocketBase(const IPProto ipProtocol, const IPVer ipVersion)
    : mIpProtocol(ipProtocol)
    , mIpVersion(ipVersion) {
    open();
}

SocketBase::SocketBase(const IPProto ipProtocol,
                       const IPVer ipVersion,
                       const SocketHandle clientFileDescriptor) noexcept
    : mSocketHandle(clientFileDescriptor)
    , mIpProtocol(ipProtocol)
    , mIpVersion(ipVersion) {}

sockaddr SocketBase::createAddr(const std::string& hostIp, const Port port) const {
    sockaddr addr = {};
    void* addrOutDestionation = nullptr;
    switch (mIpVersion) {
    case IPVer::IPV4: {
        sockaddr_in& addr4 = reinterpret_cast<sockaddr_in&>(addr);
        addr4.sin_port = htons(port);
        addr4.sin_family = AF_INET;
        addrOutDestionation = &addr4.sin_addr;
        break;
    }
    case IPVer::IPV6: {
        sockaddr_in6& addr6 = reinterpret_cast<sockaddr_in6&>(addr);
        addr6.sin6_port = htons(port);
        addr6.sin6_family = AF_INET6;
        addrOutDestionation = &addr6.sin6_addr;
        break;
    }
    default:
        MISSING_CASE_LABEL;
        throw Exception(FUNC_NAME, "Socket has invalid IP version: ", static_cast<int>(mIpVersion));
    }
    if (::inet_pton(addr.sa_family, hostIp.c_str(), addrOutDestionation) != 1) {
        throw Exception(
            FUNC_NAME, "Couldn't convert IP address string to network format - ", getLastErrorFormatted());
    }
    return addr;
}

} // namespace cpplibsocket
