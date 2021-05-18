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
        const Address addr = getAddressFromFd(socket);
        return getEndpoint(&addr.sa);
    }

    Address createAddr(const Endpoint& endpoint) {
        return createAddr(endpoint.ipVersion, endpoint.ip, endpoint.port);
    }

    Address createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port) {
        Address addr = {};
        if (!ipAddress.empty()) {
            struct addrinfo hint = {};
            hint.ai_family = toNativeDomain(ipVersion);
            hint.ai_flags = AI_NUMERICHOST;
            const AddrInfo addrInfo(ipAddress, &hint);
            if (addrInfo.empty()) {
                throw Exception(FUNC_NAME, "Invalid IP address ", ipAddress);
            }
            addrinfo* info = *std::begin(addrInfo);
            ASSERT(info->ai_family == toNativeDomain(ipVersion));
            ASSERT(info->ai_addrlen == getAddrSize(ipVersion));
            std::memcpy(&addr.sa, info->ai_addr, info->ai_addrlen);
        } else {
            switch (ipVersion) {
            case IPVer::IPV4: {
                addr.sa_in.sin_addr.s_addr = INADDR_ANY;
                break;
            }
            case IPVer::IPV6: {
                addr.sa_in6.sin6_addr = IN6ADDR_ANY_INIT;
                break;
            }
            }
        }
        getSinPort(&addr.sa) = htons(port);
        return addr;
    }

    std::string getLocalIpAddress(const IPVer ipVersion) { return Platform::getLocalIpAddress(ipVersion); }

    Optional<Address>
    resolveHostname(const std::string& hostname, const Port port, const Optional<IPVer> ipVersion) noexcept {
        struct addrinfo hint = {};
        hint.ai_family = AF_UNSPEC;
        if (ipVersion) {
            hint.ai_family = *ipVersion == IPVer::IPV4 ? AF_INET : AF_INET6;
        }
        try {
            for (struct addrinfo* info : AddrInfo(hostname, &hint)) {
                try {
                    Address addr;
                    std::memcpy(&addr.sa, info->ai_addr, info->ai_addrlen);
                    getSinPort(&addr.sa) = htons(port);
                    return addr;
                } catch (...) {
                    continue;
                }
            }
        } catch (...) {
        }
        return NullOptional;
    }

    Optional<std::string> resolveHostname(const std::string& hostname,
                                          const Optional<IPVer> ipVersion) noexcept {
        const Optional<Address> addr = resolveHostname(hostname, 0, ipVersion);
        if (!addr) {
            return NullOptional;
        }
        return getEndpoint(*addr).ip;
    }

} // namespace utils
} // namespace cpplibsocket
