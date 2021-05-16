#include "cpplibsocket/Socket.h"
#include "cpplibsocket/utils/utils.h"

namespace cpplibsocket {

SocketBase::~SocketBase() noexcept {
    try {
        if (isOpen()) {
            close();
        }
    } catch (...) {
    }
}

SocketBase& SocketBase::operator=(SocketBase&& other) noexcept {
    mSocketHandle = other.mSocketHandle;
    mIpVersion = other.mIpVersion;
    other.mSocketHandle =
        Platform::SOCKET_NULL; // So the destructor of the moved object doesn't close our socket
    return *this;
}

SocketBase::SocketBase(SocketBase&& other) noexcept {
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
    const sockaddr_storage addr = createAddr(ip, port);
    if (::bind(mSocketHandle, reinterpret_cast<const sockaddr*>(&addr), getAddrSize(mIpVersion)) != 0) {
        throw Exception(
            FUNC_NAME, "Couldn't bind address \"", ip, ":", port, "\" - ", getLastErrorFormatted());
    }

    const sockaddr_storage storage = utils::getAddressFromFd(mSocketHandle);
    return ntohs(utils::getSinPort(reinterpret_cast<const sockaddr*>(&storage)));
}

Port SocketBase::bindAll(const Port port) {
    return bind("", port);
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

sockaddr_storage SocketBase::createAddr(const std::string& hostIp, const Port port) const {
    return utils::createAddr(mIpVersion, hostIp, port);
}

} // namespace cpplibsocket
