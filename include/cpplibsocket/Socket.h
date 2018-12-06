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

/// Base for actual Socket specializations
///
/// Contains basic functions for socket manipulations.
/// This class has private constructors, it's not intended to be created as a standalone object.
class SocketBase {
public:
    virtual ~SocketBase();

    SocketBase& operator=(SocketBase&& other);

    SocketBase(SocketBase&& other);

    /// Tells whether or not the socket is open
    bool isOpen() const noexcept;

    /// Opens new socket
    /// \throws Exception if the socket is already open and in case the socket creation fails.
    void open();

    /// Closes the socket
    /// \throws Exception in case the socket isn't open or in case there was some issue while closing the
    /// socket.
    void close();

    /// Assigns an address to the socket, AKA assigns a name to the socket
    ///
    /// It is normally necessary to assign a local address to the socket before a TCP socket may receive
    /// connections (\see accept()).
    /// \param ip The IP address to assign.
    /// \param port The port to assign.
    /// \throws Exception in case the socket is not open or if the binding fails
    void bind(const std::string& ip, const Port port);

    /// Sets blocking or non-blocking property of the socket
    /// \param blocked If true, the socket will be set as blocking, if false, the socket will be non-blocking.
    /// \throws Exception in case the socket is not open or if setting the property fails.
    void setBlocked(const bool blocked = true);

protected:
    /// Creates a new socket with the given protocol and IP version
    /// \throws Exception in case there were some problems while opening the socket.
    SocketBase(const IPProto ipProtocol, const IPVer ipVersion);

    /// Creates a socket from an already open socket handle
    SocketBase(const IPProto ipProtocol,
               const IPVer ipVersion,
               const SocketHandle clientFileDescriptor) noexcept;

    /// Creates a sockaddr structure from given IP address and port
    /// \throws Exception in case there was some error while creating the structure.
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

/// RAII TCP Socket wrapper
///
/// Provides a simple interface for TCP socket manipulation
template <>
class Socket<IPProto::TCP> : public SocketBase {
public:
    /// Creates a TCP socket with the given IP version
    Socket(const IPVer ipVersion);

    /// Initiates a connection to a server
    /// \param hostIp The IP address to connect to.
    /// \param hostPort The port the server is listening on.
    /// \throws Exception in case the socket is not open or if there was some error while connecting to the
    /// peer.
    void connect(const std::string& hostIp, const Port hostPort);

    /// Starts listening for incoming connections
    /// \param backLogSize Hint to the socket determining the maximum number of outstanding connections in the
    /// socket's listen queue.
    /// \throws Exception in case the socket is not open or if there was some error while starting the
    /// listening.
    void listen(const int backLogSize);

    /// Accepts a client connection
    /// \returns A socket of the client connected
    /// \throws Exception in case the socket is not open or if the accept failed for whatever reason.
    Expected<Socket> accept() const;

    /// Sends data to the peer the socket is connected to
    /// \param data The data to send.
    /// \param size The data size.
    /// \returns If no error occurred, the size of the data sent is returned. An error is returned otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while sending the data.
    Expected<SentSize> send(const Byte* data, const DataSize size) const;

    /// Receives data from peer the socket is connected to
    /// \param data The destination for the received data.
    /// \param The maximum size of data we can receive at this time.
    /// \returns If no error occurred, the size of the data received is returned. An error is returned
    /// otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while receiving
    /// the data.
    Expected<ReceivedSize> receive(Byte* data, const DataSize maxSize) const;

private:
    /// Constructs a socket with an already open handle
    Socket(const IPVer ipVersion, const SocketHandle clientFileDescriptor);
};

/// RAII UDP Socket wrapper
///
/// Provides a simple interface for UDP socket manipulation
template <>
class Socket<IPProto::UDP> : public SocketBase {
public:
    /// Creates a UDP socket with the given IP version
    Socket(const IPVer ipVersion);

    /// Sends data to the given IP address and port
    /// \param data The data to send.
    /// \param size The data size.
    /// \param hostIp The IP the data will be sent to.
    /// \param hostPort The port the receiving socket is listening on.
    /// \returns If no error occurred, the size of the data sent is returned. An error is returned otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while sending the data.
    Expected<SentSize>
    sendTo(const Byte* data, const DataSize size, const std::string& hostIp, const Port hostPort);

    /// Receives data from the given IP address and port
    /// \param data The destination for the received data.
    /// \param The maximum size of data we can receive at this time.
    /// \returns If no error occurred, the size of the data received is returned. An error is returned
    /// otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while receiving
    /// the data.
    Expected<ReceivedSize> receiveFrom(Byte* data,
                                       const DataSize maxSize,
                                       IPVer& sourceIpVersion,
                                       std::string& sourceIp,
                                       Port& sourcePort);
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKET_H_
