#include "cpplibsocket/utils/utils.h"

#include <regex>

namespace cpplibsocket {

namespace utils {

    namespace detail {

        Port getBoundPort(SocketHandle socket) {
            sockaddr_in sin;
            SockLenType len = sizeof(sin);
            if (::getsockname(socket, reinterpret_cast<sockaddr*>(&sin), &len) != 0) {
                throw Exception(FUNC_NAME, "Couldn't get the bound port - ", getLastErrorFormatted());
            }
            return ntohs(sin.sin_port);
        }

        std::string getIpAddress(const sockaddr& addr) {
            char str[INET6_ADDRSTRLEN] = {};
            switch (toIPVer(addr.sa_family)) {
            case IPVer::IPV4:
                ::inet_ntop(
                    AF_INET, &reinterpret_cast<const sockaddr_in*>(&addr)->sin_addr, str, sizeof(str));
                return std::string(str);
            case IPVer::IPV6:
                ::inet_ntop(addr.sa_family,
                            &reinterpret_cast<const sockaddr_in6*>(&addr)->sin6_addr,
                            str,
                            sizeof(str));
                return std::string(str);
            default:
                throw Exception(FUNC_NAME, "Unknown IP version from client");
            }
        }

        Port getPort(const sockaddr& addr) {
            switch (toIPVer(addr.sa_family)) {
            case IPVer::IPV4:
                return ntohs(reinterpret_cast<const sockaddr_in*>(&addr)->sin_port);
            case IPVer::IPV6:
                return ntohs(reinterpret_cast<const sockaddr_in6*>(&addr)->sin6_port);
            default:
                throw Exception(FUNC_NAME, "Unknown IP version from client");
            }
        }

        sockaddr createAddr(const IPVer ipVersion, const std::string& hostIp, const Port port) {
            sockaddr addr = {};
            void* addrOutDestionation = nullptr;
            switch (ipVersion) {
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
                throw Exception(FUNC_NAME, "Socket has invalid IP version: ", static_cast<int>(ipVersion));
            }
            if (::inet_pton(addr.sa_family, hostIp.c_str(), addrOutDestionation) != 1) {
                throw Exception(FUNC_NAME,
                                "Couldn't convert IP address string to network format - ",
                                getLastErrorFormatted());
            }
            return addr;
        }

    } // namespace detail

    std::string getHostIpAddress(const std::string& hostName, const IPVer ipVersion) {
        static std::regex httpStripper(R"rgx(^(http[s]?:\/\/)?([^\s\/]+))rgx");
        std::smatch match;
        if (!std::regex_search(hostName, match, httpStripper) || match.size() != 3) {
            throw Exception(FUNC_NAME, "Couldn't parse the hostname format - ", hostName);
        }

        addrinfo* res = nullptr;
        addrinfo hints = {};
        hints.ai_family = toNativeDomain(ipVersion);
        const int status = getaddrinfo(match.str(2).c_str(), NULL, &hints, &res);
        auto cleanup = utils::makeFinally([&res]() {
            if (res != nullptr) {
                freeaddrinfo(res);
            }
        });

        if (status != 0) {
            throw Exception(FUNC_NAME, "Couldn't get host IP - ", gai_strerror(status));
        }

        for (addrinfo* p = res; p != nullptr; p = p->ai_next) {
            void* addr = nullptr;
            const IPVer storedIpVer = toIPVer(p->ai_family);
            if (storedIpVer == ipVersion) {
                if (storedIpVer == IPVer::IPV4) {
                    struct sockaddr_in* ipv4 = (struct sockaddr_in*)p->ai_addr;
                    addr = &(ipv4->sin_addr);
                } else if (storedIpVer == IPVer::IPV6) {
                    struct sockaddr_in6* ipv6 = (struct sockaddr_in6*)p->ai_addr;
                    addr = &(ipv6->sin6_addr);
                }
            }

            if (addr != nullptr) {
                char ip[INET6_ADDRSTRLEN];
                inet_ntop(p->ai_family, addr, ip, sizeof(ip));
                return std::string(ip);
            }
        }
        throw Exception(FUNC_NAME, "Couldn't get " + hostName + " host IP");
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

        if (::bind(s, reinterpret_cast<sockaddr*>(&addr), sizeof(sockaddr_in)) != 0) {
            throw Exception(FUNC_NAME, "Couldn't bind any port", getLastErrorFormatted());
        }

        return detail::getBoundPort(s);
    }

} // namespace utils

} // namespace cpplibsocket
