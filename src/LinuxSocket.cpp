#include "cpplibsocket/SocketCommon.h"
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
        const size_t viableSize = static_cast<size_t>(
            std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), size));
        return static_cast<SignedSize>(::send(socket, data, viableSize, 0));
    }

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr) {
        const size_t viableSize = static_cast<size_t>(
            std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), size));
        const SockLenType sockSize = getAddrSize(toIPVer(addr->sa_family));
        return static_cast<SignedSize>(::sendto(socket, data, viableSize, 0, addr, sockSize));
    }

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize maxSize) {
        const size_t viableSize = static_cast<size_t>(
            std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), maxSize));
        return static_cast<SignedSize>(::recv(socket, data, viableSize, 0));
    }

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize maxSize, sockaddr* addr) {
        const size_t viableSize = static_cast<size_t>(
            std::min(static_cast<UnsignedSize>(std::numeric_limits<size_t>::max()), maxSize));
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

    bool closeSocket(SocketHandle socket) { return ::close(socket) == 0; }

    std::string getLocalIpAddress(const IPVer ipVersion) {
        ifaddrs* addr = nullptr;
        auto cleanup = utils::makeFinally([&addr]() {
            if (addr != nullptr) {
                freeifaddrs(addr);
            }
        });
        if (getifaddrs(&addr) == -1) {
            throw Exception(FUNC_NAME, "Couldn't get local IP address - ", getLastErrorFormatted());
        }

        for (ifaddrs* walk = addr; walk != nullptr; walk = walk->ifa_next) {
            if (!walk->ifa_addr) {
                continue;
            }
            const int family = walk->ifa_addr->sa_family;
            if (family != toNativeDomain(ipVersion) || !(walk->ifa_flags & IFF_UP) ||
                (walk->ifa_flags & IFF_LOOPBACK)) {
                continue;
            }

            char addrRaw[NI_MAXHOST] = {};
            const SockLenType sockSize = family == AF_INET ? sizeof(sockaddr_in) : sizeof(sockaddr_in6);

            const int status =
                getnameinfo(walk->ifa_addr, sockSize, addrRaw, sizeof(addrRaw), nullptr, 0, NI_NUMERICHOST);
            if (status != 0) {
                throw Exception(FUNC_NAME, "Couldn't get local IP address - ", gai_strerror(status));
            }
            return std::string(addrRaw);
        }

        throw Exception(FUNC_NAME, "Couldn't get local IP address");
    }

} // namespace Platform

} // namespace cpplibsocket
