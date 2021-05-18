#ifndef CPPLIBSOCKET_UTILS_ENDPOINTPRINT_H_
#define CPPLIBSOCKET_UTILS_ENDPOINTPRINT_H_

#include "cpplibsocket/SocketCommon.h"

#include <iostream>

inline std::ostream& operator<<(std::ostream& o, const cpplibsocket::Endpoint& endpoint) {
    o << (endpoint.ipVersion == cpplibsocket::IPVer::IPV4
              ? (endpoint.ip.empty() ? "0.0.0.0" : endpoint.ip)
              : (endpoint.ip.empty() ? "[::/0]" : ("[" + endpoint.ip + "]")))
      << ':' << endpoint.port;
    return o;
}

#endif // CPPLIBSOCKET_UTILS_ENDPOINTPRINT_H_
