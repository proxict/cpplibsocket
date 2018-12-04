#include "cpplibsocket/LinuxSocket.h"

#include <limits>

namespace cpplibsocket {

std::string getLastErrorFormatted() {
    return std::strerror(errno);
}

namespace Platform {

    DataSize send(SocketHandle socket, const Byte* data, const DataSize size) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<DataSize>(std::numeric_limits<size_t>::max()), size));
        return static_cast<DataSize>(::send(socket, data, viableSize, 0));
    }

    DataSize sendTo(SocketHandle socket, const Byte* data, const DataSize size, const sockaddr* addr) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<DataSize>(std::numeric_limits<size_t>::max()), size));
        const SockLenType sockSize = getAddrSize(toIPVer(addr->sa_family));
        return static_cast<DataSize>(::sendto(socket, data, viableSize, 0, addr, sockSize));
    }

    DataSize receive(SocketHandle socket, Byte* data, const DataSize maxSize) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<DataSize>(std::numeric_limits<size_t>::max()), maxSize));
        return static_cast<DataSize>(::recv(socket, data, viableSize, 0));
    }

    DataSize receiveFrom(SocketHandle socket, Byte* data, const DataSize maxSize, sockaddr* addr) {
        const size_t viableSize =
            static_cast<size_t>(std::min(static_cast<DataSize>(std::numeric_limits<size_t>::max()), maxSize));
        SockLenType dummy;
        return static_cast<DataSize>(::recvfrom(socket, data, viableSize, 0, addr, &dummy));
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
