#include "cpplibsocket/SocketCommon.h"
#include "cpplibsocket/utils/Defer.h"
#include "cpplibsocket/utils/utils.h"

#include <fcntl.h>
#include <ifaddrs.h>
#include <limits>
#include <net/if.h>
#include <netdb.h>
#include <unistd.h>

namespace cpplibsocket {

std::string getLastErrorFormatted() {
    return std::strerror(errno);
}

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size) {
        const std::size_t viableSize = std::min(std::numeric_limits<size_t>::max(), size);
        return ::send(socket, data, viableSize, 0);
    }

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr) {
        const std::size_t viableSize = std::min(std::numeric_limits<size_t>::max(), size);
        const SockLenType sockSize = getAddrSize(toIPVer(addr->sa_family));
        return ::sendto(socket, data, viableSize, 0, addr, sockSize);
    }

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize maxSize) {
        const std::size_t viableSize = std::min(std::numeric_limits<size_t>::max(), maxSize);
        return ::recv(socket, data, viableSize, 0);
    }

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize maxSize, sockaddr* addr) {
        const std::size_t viableSize = std::min(std::numeric_limits<size_t>::max(), maxSize);
        SockLenType sockSize = sizeof(Address);
        return ::recvfrom(socket, data, viableSize, 0, addr, &sockSize);
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

    bool closeSocket(SocketHandle socket) { return ::close(socket) == 0; }

} // namespace Platform

} // namespace cpplibsocket
