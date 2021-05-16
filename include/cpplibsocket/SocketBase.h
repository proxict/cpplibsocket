#ifndef CPPLIBSOCKET_SOCKETBASE_H_
#define CPPLIBSOCKET_SOCKETBASE_H_

#include "cpplibsocket/SocketCommon.h"
#include "cpplibsocket/common/Assert.h"
#include "cpplibsocket/utils/AnyOf.h"
#include "cpplibsocket/utils/Expected.h"
#include "cpplibsocket/utils/Flags.h"

namespace cpplibsocket {

/// Base for actual Socket specializations
///
/// Contains basic functions for socket manipulations.
/// This class has private constructors, it's not intended to be created as a standalone object.
class SocketBase {
public:
    virtual ~SocketBase() noexcept;

    SocketBase& operator=(SocketBase&& other) noexcept;

    SocketBase(SocketBase&& other) noexcept;

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
    /// \param port The port to assign. If the port is 0, any free local port will be used.
    /// \returns Port which was bound to the socket
    /// \throws Exception in case the socket is not open or if the binding fails
    Port bind(const std::string& ip, const Port port = 0);

    /// Binds the socket to all interfaces.
    ///
    /// It is normally necessary to assign a local address to the socket before a TCP socket may receive
    /// connections (\see accept()).
    /// \param port The port to assign. If the port is 0, any free local port will be used.
    /// \returns Port which was bound to the socket
    /// \throws Exception in case the socket is not open or if the binding fails
    Port bindAll(const Port port = 0);

    /// Sets blocking or non-blocking property of the socket
    /// \param blocked If true, the socket will be set as blocking, if false, the socket will be non-blocking.
    /// \throws Exception in case the socket is not open or if setting the property fails.
    void setBlocked(const bool blocked = true);

    /// Sets timeout for receiving data from the socket
    /// \param timeout The timeout to set.
    /// \direction The direction to set the timeout for.
    /// \throws Exception in case the socket is not open or if setting the timeout fails.
    template <typename TRep, typename TPeriod>
    void setTimeout(const std::chrono::duration<TRep, TPeriod> timeout,
                    const utils::Flags<Direction> direction = Direction::TX | Direction::RX) {
        if (!isOpen()) {
            throw Exception(FUNC_NAME, "The socket is not open");
        }
        if (direction.isSet(Direction::TX) && !Platform::setTimeout(mSocketHandle, SO_SNDTIMEO, timeout)) {
            throw Exception(FUNC_NAME, "Couldn't set socket timeout - ", getLastErrorFormatted());
        }
        if (direction.isSet(Direction::RX) && !Platform::setTimeout(mSocketHandle, SO_RCVTIMEO, timeout)) {
            throw Exception(FUNC_NAME, "Couldn't set socket timeout - ", getLastErrorFormatted());
        }
    }

    SocketHandle getSocketHandle() const noexcept { return mSocketHandle; }

protected:
    /// Creates a new socket with the given protocol and IP version
    /// \throws Exception in case there were some problems while opening the socket.
    SocketBase(const IPProto ipProtocol, const IPVer ipVersion);

    /// Creates a socket from an already open socket handle
    /// \param ipProtocol The protocol of the client socket.
    /// \param ipVersion The version of the client socket.
    /// \param clientSocketHandle The handle of the client socket.
    SocketBase(const IPProto ipProtocol,
               const IPVer ipVersion,
               const SocketHandle clientSocketHandle) noexcept;

    /// Creates a sockaddr structure from given IP address and port
    /// \throws Exception in case there was some error while creating the structure.
    sockaddr_storage createAddr(const std::string& hostIp, const Port port) const;

    SocketHandle mSocketHandle = Platform::SOCKET_NULL;
    IPProto mIpProtocol;
    IPVer mIpVersion;

private:
    SocketBase(const SocketBase&) = delete;
    SocketBase& operator=(const SocketBase&) = delete;
};

template <IPProto TIPProto>
class Socket;

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_SOCKETBASE_H_
