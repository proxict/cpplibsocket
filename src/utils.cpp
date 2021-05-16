#include "cpplibsocket/utils/utils.h"
#include "cpplibsocket/utils/Defer.h"

namespace cpplibsocket {

namespace utils {

    sockaddr_storage getAddressFromFd(SocketHandle socket) {
        sockaddr_storage addr = {};
        SockLenType len = sizeof(addr);
        if (::getsockname(socket, reinterpret_cast<sockaddr*>(&addr), &len) != 0) {
            throw Exception(FUNC_NAME, "Couldn't get address from socket - ", getLastErrorFormatted());
        }
        return addr;
    }

    Endpoint getEndpoint(const sockaddr* addr) {
        char str[INET6_ADDRSTRLEN] = {};
        ::inet_ntop(addr->sa_family, getSinAddr(addr), str, sizeof(str));
        return Endpoint(toIPVer(addr->sa_family), std::string(str), ntohs(getSinPort(addr)));
    }

    Endpoint getEndpoint(SocketHandle socket) {
        const sockaddr_storage storage = getAddressFromFd(socket);
        return getEndpoint(reinterpret_cast<const sockaddr*>(&storage));
    }

    sockaddr_storage createAddr(const Endpoint& endpoint) {
        return createAddr(endpoint.ipVersion, endpoint.ip, endpoint.port);
    }

    sockaddr_storage createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port) {
        struct sockaddr_storage storage = {};
        sockaddr* addr = reinterpret_cast<sockaddr*>(&storage);
        storage.ss_family = toNativeDomain(ipVersion);
        getSinPort(addr) = htons(port);

        if (!ipAddress.empty()) {
            if (inet_pton(storage.ss_family, ipAddress.c_str(), getSinAddr(addr)) != 1) {
                throw Exception(FUNC_NAME, "Invalid IP address ", ipAddress);
            }
        } else {
            switch (ipVersion) {
            case IPVer::IPV4: {
                traits::castAddrPointer<IPVer::IPV4>(addr)->sin_addr.s_addr = INADDR_ANY;
                break;
            }
            case IPVer::IPV6: {
                traits::castAddrPointer<IPVer::IPV6>(addr)->sin6_addr = IN6ADDR_ANY_INIT;
                break;
            }
            }
        }

        return storage;
    }

    std::string getLocalIpAddress(const IPVer ipVersion) { return Platform::getLocalIpAddress(ipVersion); }

} // namespace utils

} // namespace cpplibsocket
