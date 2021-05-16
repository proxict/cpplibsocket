#ifndef CPPLIBSOCKET_SOCKETTCP_H_
#define CPPLIBSOCKET_SOCKETTCP_H_

#include "cpplibsocket/SocketBase.h"

namespace cpplibsocket {

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
    /// \param backlogSize Hint to the socket determining the maximum number of outstanding connections in the
    /// socket's listen queue.
    /// \throws Exception in case the socket is not open or if there was some error while starting the
    /// listening.
    void listen(const int backlogSize);

    /// Accepts a client connection
    /// \returns A socket of the client connected
    /// \throws Exception in case the socket is not open or if the accept failed for whatever reason.
    Expected<Socket> accept() const;

    /// Sends data to the peer the socket is connected to
    /// \param data The data to send.
    /// \param size The data size.
    /// \returns If no error occurred, the size of the data sent is returned. An error is returned otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while sending the data.
    Expected<UnsignedSize> send(const Byte* data, const UnsignedSize size) const;

    /// Receives data from peer the socket is connected to
    /// \param data The destination for the received data.
    /// \param The maximum size of data we can receive at this time.
    /// \returns If no error occurred, the size of the data received is returned. An error is returned
    /// otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while receiving
    /// the data.
    Expected<UnsignedSize> receive(Byte* data, const UnsignedSize maxSize) const;

private:
    Socket(const IPVer ipVersion, const SocketHandle clientSocketHandle) noexcept;
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKETTCP_H_
