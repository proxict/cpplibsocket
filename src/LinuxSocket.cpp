#include "cpplibsocket/LinuxSocket.h"

#include <limits>

namespace cpplibsocket {

std::string getLastErrorFormatted() {
    return std::strerror(errno);
}

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), size));
        return static_cast<SignedSize>(::send(socket, data, viableSize, 0));
    }

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), size));
        const SockLenType sockSize = getAddrSize(toIPVer(addr->sa_family));
        return static_cast<SignedSize>(::sendto(socket, data, viableSize, 0, addr, sockSize));
    }

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize maxSize) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), maxSize));
        return static_cast<SignedSize>(::recv(socket, data, viableSize, 0));
    }

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize maxSize, sockaddr* addr) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), maxSize));
        SockLenType dummy = sizeof(sockaddr);
        return static_cast<SignedSize>(::recvfrom(socket, data, viableSize, 0, addr, &dummy));
    }

    bool setBlocked(SocketHandle socket, const bool blocked) {
        int flags = fcntl(socket, F_GETFL, 0);
        if (flags == -1) {
            return false;
        }
        flags = blocked ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);

        if (fcntl(socket, F_SETFL, flags) == -1) {
            return false;
        }
        return true;
    }

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion) {
        return ::socket(toNativeDomain(ipVersion), toNativeType(ipProtocol), toNativeProtocol(ipProtocol));
    }

    int closeSocket(SocketHandle socket) { return ::close(socket) == 0; }

} // namespace Platform

} // namespace cpplibsocket
