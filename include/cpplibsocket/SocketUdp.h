#ifndef CPPLIBSOCKET_SOCKETUDP_H_
#define CPPLIBSOCKET_SOCKETUDP_H_

#include "cpplibsocket/SocketBase.h"

namespace cpplibsocket {

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
    Expected<UnsignedSize>
    sendTo(const Byte* data, const UnsignedSize size, const std::string& hostIp, const Port hostPort);

    /// Receives data from the given IP address and port
    /// \param data The destination for the received data.
    /// \param The maximum size of data we can receive at this time.
    /// \returns If no error occurred, the size of the data received is returned. An error is returned
    /// otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while receiving
    /// the data.
    Expected<UnsignedSize> receiveFrom(Byte* data, const UnsignedSize maxSize);

    /// Receives data from the given IP address and port
    /// \param data The destination for the received data.
    /// \param The maximum size of data we can receive at this time.
    /// \param sourceIpVersion After the function call, the version of IP address of the sender will be saved
    /// in this argument.
    /// \param sourceIpVersion After the function call, the IP address of the sender will be saved in this
    /// argument.
    /// \param sourceIpPort After the function call, the port of the sender will be saved in this argument.
    /// \returns If no error occurred, the size of the data received is returned. An error is returned
    /// otherwise.
    /// \throws Exception in case the socket is not open or if there was some error while receiving the data.
    Expected<UnsignedSize> receiveFrom(Byte* data,
                                       const UnsignedSize maxSize,
                                       IPVer& sourceIpVersion,
                                       std::string& sourceIp,
                                       Port& sourcePort);
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKETUDP_H_
