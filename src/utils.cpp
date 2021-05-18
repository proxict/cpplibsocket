#include "cpplibsocket/utils/utils.h"
#include "cpplibsocket/utils/Defer.h"

namespace cpplibsocket {
namespace utils {

    AddrInfo::AddrInfo(const std::string& address, struct addrinfo* hint)
        : mInfo(nullptr) {
        const int status = getaddrinfo(address.c_str(), nullptr, hint, &mInfo);
        if (status != 0) {
            throw Exception(
                FUNC_NAME, "Couldn't get info for address \"", address, "\" - ", gai_strerror(status));
        }
    }

    bool AddrInfo::empty() const { return mInfo == nullptr; }

    AddrInfo::~AddrInfo() noexcept { ::freeaddrinfo(mInfo); }

    AddrInfo::Iterator AddrInfo::begin() const { return Iterator(mInfo); }

    AddrInfo::Iterator AddrInfo::end() const { return Iterator(nullptr); }

    Address getAddressFromFd(SocketHandle socket) {
        Address addr = {};
        SockLenType len = sizeof(addr);
        if (::getsockname(socket, &addr.sa, &len) != 0) {
            throw Exception(FUNC_NAME, "Couldn't get address from socket - ", getLastErrorFormatted());
        }
        return addr;
    }

    Endpoint getEndpoint(const sockaddr* addr) {
        char str[INET6_ADDRSTRLEN] = {};
        ::inet_ntop(addr->sa_family, getSinAddr(addr), str, sizeof(str));
        return Endpoint(toIPVer(addr->sa_family), std::string(str), ntohs(getSinPort(addr)));
    }

    Endpoint getEndpoint(const Address& address) { return getEndpoint(&address.sa); }

    Endpoint getEndpoint(SocketHandle socket) {
        const Address storage = getAddressFromFd(socket);
        return getEndpoint(&storage.sa);
    }

    Address createAddr(const Endpoint& endpoint) {
        return createAddr(endpoint.ipVersion, endpoint.ip, endpoint.port);
    }

    Address createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port) {
        Address storage = {};
        storage.sa_stor.ss_family = toNativeDomain(ipVersion);
        getSinPort(&storage.sa) = htons(port);

        if (!ipAddress.empty()) {
            struct addrinfo hint = {};
            hint.ai_family = storage.sa_stor.ss_family;
            hint.ai_flags = AI_NUMERICHOST;
            const AddrInfo addrInfo(ipAddress, &hint);
            if (addrInfo.empty()) {
                throw Exception(FUNC_NAME, "Invalid IP address ", ipAddress);
            }
            addrinfo* info = *std::begin(addrInfo);
            ASSERT(info->ai_family == toNativeDomain(ipVersion));
            ASSERT(info->ai_addrlen == getAddrSize(ipVersion));
            const std::size_t offset = ipVersion == IPVer::IPV4 ? offsetof(sockaddr_in, sin_addr)
                                                                : offsetof(sockaddr_in6, sin6_addr);
            std::memcpy(getSinAddr(&storage.sa), getSinAddr(info->ai_addr), info->ai_addrlen - offset);
        } else {
            switch (ipVersion) {
            case IPVer::IPV4: {
                storage.sa_in.sin_addr.s_addr = INADDR_ANY;
                break;
            }
            case IPVer::IPV6: {
                storage.sa_in6.sin6_addr = IN6ADDR_ANY_INIT;
                break;
            }
            }
        }

        return storage;
    }

    std::string getLocalIpAddress(const IPVer ipVersion) { return Platform::getLocalIpAddress(ipVersion); }

    Optional<std::string> resolveHostname(const std::string& hostname,
                                          const Optional<IPVer> ipVersion) noexcept {
        struct addrinfo hint = {};
        hint.ai_family = AF_UNSPEC;
        if (ipVersion) {
            hint.ai_family = *ipVersion == IPVer::IPV4 ? AF_INET : AF_INET6;
        }
        try {
            AddrInfo addrInfo(hostname, &hint);
            for (struct addrinfo* info : addrInfo) {
                try {
                    return getEndpoint(info->ai_addr).ip;
                } catch (...) {
                    continue;
                }
            }
        } catch (...) {
        }
        return NullOptional;
    }

} // namespace utils
} // namespace cpplibsocket
