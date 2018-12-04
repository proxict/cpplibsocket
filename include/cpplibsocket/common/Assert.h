#ifndef CPPLIBSOCKET_COMMON_ASSERT_H_
#define CPPLIBSOCKET_COMMON_ASSERT_H_

#include <cassert>

namespace cpplibsocket {

#define ASSERT(x) assert((x))
#define MISSING_CASE_LABEL ASSERT(!"Missing case label")

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_COMMON_ASSERT_H_
