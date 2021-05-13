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

    sockaddr_storage createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port) {
        struct sockaddr_storage storage = {};
        storage.ss_family = toNativeDomain(ipVersion);
        const int status = inet_pton(
            storage.ss_family, ipAddress.c_str(), getSinAddr(reinterpret_cast<sockaddr*>(&storage)));
        getSinPort(reinterpret_cast<sockaddr*>(&storage)) = htons(port);
        if (status != 1) {
            throw Exception(FUNC_NAME, "Invalid IP address ", ipAddress);
        }
        return storage;
    }

    std::string getLocalIpAddress(const IPVer ipVersion) { return Platform::getLocalIpAddress(ipVersion); }

    Port getFreePort() {
        SocketHandle s = ::socket(AF_INET, SOCK_DGRAM, 0);
        if (s == Platform::SOCKET_NULL) {
            throw Exception(FUNC_NAME, "Couldn't open socket - ", getLastErrorFormatted());
        }

        sockaddr_in addr = {};
        addr.sin_port = 0;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_family = AF_INET;

        if (::bind(s, reinterpret_cast<const sockaddr*>(&addr), sizeof(sockaddr_in)) != 0) {
            throw Exception(FUNC_NAME, "Couldn't bind any port", getLastErrorFormatted());
        }

        const sockaddr_storage storage = getAddressFromFd(s);
        return ntohs(getSinPort(reinterpret_cast<const sockaddr*>(&storage)));
    }

} // namespace utils

} // namespace cpplibsocket
