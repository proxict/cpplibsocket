#ifndef CPPLIBSOCKET_COMMON_COMMON_H_
#define CPPLIBSOCKET_COMMON_COMMON_H_

#include <cstdint>

#define FUNC_NAME __func__, "(): "

namespace cpplibsocket {

using Byte = uint8_t;
using Size = std::size_t;
using Port = uint16_t;

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_COMMON_COMMON_H_
