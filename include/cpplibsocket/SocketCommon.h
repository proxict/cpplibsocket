#ifndef CPPLIBSOCKET_SOCKETCOMMON_H_
#define CPPLIBSOCKET_SOCKETCOMMON_H_

#include "cpplibsocket/common/Exception.h"
#include "cpplibsocket/common/common.h"
#include "cpplibsocket/common/Assert.h"

#include <cstring>
#include <type_traits>

#ifdef _WIN32
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <WS2tcpip.h>
#include <winsock2.h>
#else
#include <arpa/inet.h>
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

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKETCOMMON_H_
