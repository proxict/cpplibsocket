#ifndef CPPLIBSOCKET_SOCKETCOMMON_H_
#define CPPLIBSOCKET_SOCKETCOMMON_H_

#include "cpplibsocket/common/Assert.h"
#include "cpplibsocket/common/Exception.h"
#include "cpplibsocket/common/common.h"

#include <chrono>
#include <cstring>
#include <type_traits>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
#include <netdb.h>
#include <sys/socket.h>
#endif

namespace cpplibsocket {

#ifdef _WIN32
using SockLenType = int;
using SocketHandle = SOCKET;
#else
using SockLenType = socklen_t;
using SocketHandle = int;
#endif

using UnsignedSize = std::size_t;
using SignedSize = typename std::make_signed<UnsignedSize>::type;

enum class IPProto : int { TCP, UDP };

enum class IPVer : int { IPV4, IPV6 };

enum class Direction : int { TX, RX };

struct WouldBlock {};

struct Endpoint {
    Endpoint() noexcept = default;

    Endpoint(const IPVer ipVer, std::string ipAddress, const Port port_) noexcept
        : ipVersion(ipVer)
        , ip(std::move(ipAddress))
        , port(port_) {}

    IPVer ipVersion;
    std::string ip;
    Port port;
};

inline IPVer toIPVer(const int nativeIpVersion) {
    switch (nativeIpVersion) {
    case AF_INET:
        return IPVer::IPV4;
    case AF_INET6:
        return IPVer::IPV6;
    default:
        throw Exception(FUNC_NAME, "Unknown IP version passed", nativeIpVersion);
    }
}

inline SockLenType getAddrSize(const IPVer ipVersion) {
    switch (ipVersion) {
    case IPVer::IPV4:
        return sizeof(sockaddr_in);
    case IPVer::IPV6:
        return sizeof(sockaddr_in6);
    default:
        throw Exception(FUNC_NAME, "Invalid IP version passed: ", static_cast<int>(ipVersion));
    }
}

inline int toNativeDomain(const IPVer ipVersion) {
    switch (ipVersion) {
    case IPVer::IPV4:
        return AF_INET;
    case IPVer::IPV6:
        return AF_INET6;
    default:
        MISSING_CASE_LABEL;
        throw Exception(FUNC_NAME, "Invalid IP version passed: ", static_cast<int>(ipVersion));
    }
}

inline int toNativeType(const IPProto protocol) {
    switch (protocol) {
    case IPProto::TCP:
        return SOCK_STREAM;
    case IPProto::UDP:
        return SOCK_DGRAM;
    default:
        MISSING_CASE_LABEL;
        throw Exception(FUNC_NAME, "Invalid IP protocol passed: ", static_cast<int>(protocol));
    }
}

inline int toNativeProtocol(const IPProto protocol) {
    switch (protocol) {
    case IPProto::TCP:
        return IPPROTO_TCP;
    case IPProto::UDP:
        return IPPROTO_UDP;
    default:
        MISSING_CASE_LABEL;
        throw Exception(FUNC_NAME, "Invalid IP protocol passed: ", static_cast<int>(protocol));
    }
}

/// Returns a string representation of the last error that occurred in either system calls or in some library
/// functions.
std::string getLastErrorFormatted();

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size);

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr);

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize size);

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize size, sockaddr* addr);

    bool setBlocked(SocketHandle socket, const bool blocked = true);

    template <typename TRep, typename TPeriod>
    bool
    setTimeout(SocketHandle socket, const int direction, const std::chrono::duration<TRep, TPeriod> timeout) {
#ifdef _WIN32
        const DWORD timeoutMs = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
        const DWORD optname = static_cast<DWORD>(direction);
        const char* optval = static_cast<const char*>(&timeoutMs);
        return setsockopt(socket, SOL_SOCKET, optname, optval, sizeof(DWORD)) != SOCKET_ERROR;
#else
        const auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count();
        const timeval tv{ milliseconds / 1000, (milliseconds % 1000) * 1000 };
        return setsockopt(socket, SOL_SOCKET, direction, &tv, sizeof(tv)) != -1;
#endif
    }

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion);

    bool closeSocket(SocketHandle socket);

    std::string getLocalIpAddress(const IPVer ipVersion);

#ifdef _WIN32
    static constexpr SocketHandle SOCKET_NULL = INVALID_SOCKET;
#else
    static constexpr SocketHandle SOCKET_NULL = -1;
#endif

} // namespace Platform

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKETCOMMON_H_
