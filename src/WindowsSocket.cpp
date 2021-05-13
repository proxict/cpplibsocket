#include "cpplibsocket/SocketCommon.h"
#include "cpplibsocket/utils/utils.h"

#include <algorithm>
#include <limits>
#include <mutex>
#include <set>
#include <vector>

#include <iphlpapi.h>

class WinSockInitializer {
    bool mInitialized;

public:
    WinSockInitializer()
        : mInitialized(false) {
        WSAData wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            throw;
        }
        mInitialized = true;
    }

    ~WinSockInitializer() {
        if (mInitialized) {
            WSACleanup();
        }
    }
};

static WinSockInitializer gWinsockInitializer;

namespace cpplibsocket {

std::string getErrorString(const DWORD errorId) {
    LPSTR output = nullptr;
    const DWORD ret = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM,
                                     nullptr,
                                     errorId,
                                     MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                     (LPSTR)&output,
                                     0,
                                     nullptr);
    std::string str;
    if (ret > 0) {
        str = output;
    } else {
        str = "Unknown error";
    }
    if (output) {
        LocalFree(output);
    }
    return str;
}

std::string getLastErrorFormatted() {
    return getErrorString(WSAGetLastError());
}

namespace Platform {

    SignedSize send(SocketHandle socket, const Byte* data, const UnsignedSize size) {
        const int viableSize =
            static_cast<int>(std::min(static_cast<UnsignedSize>(std::numeric_limits<int>::max()), size));
        return static_cast<SignedSize>(::send(socket, reinterpret_cast<const char*>(data), viableSize, 0));
    }

    SignedSize sendTo(SocketHandle socket, const Byte* data, const UnsignedSize size, const sockaddr* addr) {
        const int viableSize =
            static_cast<int>(std::min(static_cast<UnsignedSize>(std::numeric_limits<int>::max()), size));
        const SockLenType sockSize = getAddrSize(toIPVer(addr->sa_family));
        return static_cast<SignedSize>(
            ::sendto(socket, reinterpret_cast<const char*>(data), viableSize, 0, addr, sockSize));
    }

    SignedSize receive(SocketHandle socket, Byte* data, const UnsignedSize maxSize) {
        const int viableSize =
            static_cast<int>(std::min(static_cast<UnsignedSize>(std::numeric_limits<int>::max()), maxSize));
        return static_cast<SignedSize>(::recv(socket, reinterpret_cast<char*>(data), viableSize, 0));
    }

    SignedSize receiveFrom(SocketHandle socket, Byte* data, const UnsignedSize maxSize, sockaddr* addr) {
        const int viableSize =
            static_cast<int>(std::min(static_cast<UnsignedSize>(std::numeric_limits<int>::max()), maxSize));
        SockLenType sockSize = sizeof(sockaddr_storage);
        return static_cast<SignedSize>(
            ::recvfrom(socket, reinterpret_cast<char*>(data), viableSize, 0, addr, &sockSize));
    }

    bool setBlocked(SocketHandle socket, const bool blocked) {
        u_long mode = blocked ? 0U : 1U;
        return ioctlsocket(socket, FIONBIO, &mode) != SOCKET_ERROR;
    }

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion) {
        return ::socket(toNativeDomain(ipVersion), toNativeType(ipProtocol), toNativeProtocol(ipProtocol));
    }

    bool closeSocket(SocketHandle socket) {
        return ::closesocket(socket) != SOCKET_ERROR;
    }

    std::string getLocalIpAddress(const IPVer ipVersion) {
        std::vector<IP_ADAPTER_ADDRESSES> adapterAddresses;
        ULONG adapterCnt = 0;
        auto getAdapters = [&adapterAddresses, &adapterCnt, ipVersion] {
            return GetAdaptersAddresses((ULONG)toNativeDomain(ipVersion),
                                        GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER,
                                        nullptr,
                                        adapterAddresses.data(),
                                        &adapterCnt);
        };

        // Get just the number of adapters
        ULONG retval = getAdapters();
        if (retval != ERROR_BUFFER_OVERFLOW || adapterCnt == 0) {
            throw Exception(FUNC_NAME, "Couldn't get local IP address - ", getLastErrorFormatted());
        }
        adapterAddresses.resize(adapterCnt);

        // Now get the actual adresses
        retval = getAdapters();
        if (retval != ERROR_SUCCESS) {
            throw Exception(FUNC_NAME, "Couldn't get local IP address - ", getLastErrorFormatted());
        }

        for (IP_ADAPTER_ADDRESSES* item = adapterAddresses.data(); item; item = item->Next) {
            if (item->OperStatus != IfOperStatusUp) {
                continue;
            }
            return utils::detail::getIpAddress(*item->FirstUnicastAddress->Address.lpSockaddr);
        }

        throw Exception(FUNC_NAME, "Couldn't get local IP address");
    }

} // namespace Platform

} // namespace cpplibsocket
