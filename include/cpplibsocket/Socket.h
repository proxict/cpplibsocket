#ifndef CPPLIBSOCKET_SOCKET_H_
#define CPPLIBSOCKET_SOCKET_H_

#include "cpplibsocket/common/Assert.h"
#include "cpplibsocket/utils/AnyOf.h"
#include "cpplibsocket/utils/Expected.h"

#ifdef _WIN32
#include "cpplibsocket/WindowsSocket.h"
#else
#include "cpplibsocket/LinuxSocket.h"
#endif

namespace cpplibsocket {

class SocketBase {
public:
    virtual ~SocketBase();

    SocketBase& operator=(SocketBase&& other);

    SocketBase(SocketBase&& other);

    bool isOpen() const;

    void open();

    void close();

    void bind(const std::string& ip, const Port port);

    void setBlocked(const bool blocked = true);

protected:
    SocketBase(const IPProto ipProtocol, const IPVer ipVersion);

    SocketBase(const IPProto ipProtocol, const IPVer ipVersion, const SocketHandle clientFileDescriptor);

    sockaddr createAddr(const std::string& hostIp, const Port port) const;

    SocketHandle mSocketHandle = Platform::SOCKET_NULL;
    IPProto mIpProtocol;
    IPVer mIpVersion;

private:
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;
};

template <IPProto TIPProto>
class Socket;

template <>
class Socket<IPProto::TCP> : public SocketBase {
public:
    Socket(const IPVer ipVersion);

    Socket(const IPVer ipVersion, const SocketHandle clientFileDescriptor);

    void connect(const std::string& hostIp, const Port hostPort);

    void listen(const int backLogSize);

    Expected<Socket> accept() const;

    Expected<SentSize> send(const Byte* data, const DataSize size) const;

    Expected<ReceivedSize> receive(Byte* data, const DataSize maxSize) const;
};

template <>
class Socket<IPProto::UDP> : public SocketBase {
public:
    Socket(const IPVer ipVersion);

    Expected<SentSize>
    sendTo(const Byte* data, const DataSize size, const std::string& hostIp, const Port hostPort);

    Expected<ReceivedSize> receiveFrom(Byte* data,
                                       const DataSize maxSize,
                                       IPVer& sourceIpVersion,
                                       std::string& sourceIp,
                                       Port& sourcePort);
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKET_H_
