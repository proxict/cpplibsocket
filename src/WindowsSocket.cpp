#include "cpplibsocket/WindowsSocket.h"

#include <algorithm>
#include <limits>
#include <mutex>
#include <set>

namespace cpplibsocket {

/// In Windows, before using any socket functionality, WinSocket library must be initialized. The same way,
/// after the work with WinSockets is done, the library must be cleaned up properly. To ensure this is done
/// correctly and hidden from users of this library, global storage for explicitly created sockets is
/// introduced. We cannot use a simple counter here as some functions can create a socket for us, bypassing
/// the counter incrementation, but we still need to take care of their destruction, thus the counter would be
/// decremented up on the socket destruction, which could lead to unintentional library cleanup before it's
/// actually needed. This solution is also thread safe.
static std::set<SocketHandle> gAllocatedSockets;
static std::mutex gSocketMutex;

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

bool initializeWinsock() {
    ASSERT(gAllocatedSockets.empty());
    WSAData wsaData;
    return WSAStartup(MAKEWORD(2, 2), &wsaData) == 0;
}

void cleanUpWinsock() {
    ASSERT(gAllocatedSockets.empty());
    WSACleanup();
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
        SockLenType dummy;
        return static_cast<SignedSize>(
            ::recvfrom(socket, reinterpret_cast<char*>(data), viableSize, 0, addr, &dummy));
    }

    bool setBlocked(SocketHandle socket, const bool blocked) {
        u_long mode = blocked ? 0U : 1U;
        return ioctlsocket(socket, FIONBIO, &mode) != SOCKET_ERROR;
    }

    SocketHandle openSocket(const IPProto ipProtocol, const IPVer ipVersion) {
        std::lock_guard<std::mutex> lock(gSocketMutex);
        if (gAllocatedSockets.empty()) {
            if (!initializeWinsock()) {
                throw Exception(FUNC_NAME, "Couldn't initialize WinSockets - ", getLastErrorFormatted());
            }
        }
        const SocketHandle socket =
            ::socket(toNativeDomain(ipVersion), toNativeType(ipProtocol), toNativeProtocol(ipProtocol));
        if (socket != SOCKET_NULL) {
            gAllocatedSockets.insert(socket);
        }
        return socket;
    }

    bool closeSocket(SocketHandle socket) {
        const int ret = ::closesocket(socket);
        std::lock_guard<std::mutex> lock(gSocketMutex);
        if (ret != SOCKET_ERROR) {
            const auto closedSocket = gAllocatedSockets.find(socket);
            if (closedSocket != gAllocatedSockets.end()) {
                gAllocatedSockets.erase(closedSocket);
            }
            if (gAllocatedSockets.empty()) {
                cleanUpWinsock();
            }
        }
        return ret != SOCKET_ERROR;
    }

} // namespace Platform

} // namespace cpplibsocket
