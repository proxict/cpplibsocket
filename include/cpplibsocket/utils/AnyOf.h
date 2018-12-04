#ifndef CPPLIBSOCKET_UTILS_ANYOF_H_
#define CPPLIBSOCKET_UTILS_ANYOF_H_

#include <array>
#include <algorithm>

namespace cpplibsocket {

template <typename T, int TSize>
struct AnyOfThis {
    template <typename TFirst, typename... TOthers>
    explicit AnyOfThis(TFirst&& first, TOthers&&... others)
        : values({ std::forward<TFirst>(first), std::forward<TOthers>(others)... }) {}

    std::array<T, TSize> values;
};

template <typename TFirst, typename... TOthers>
auto anyOf(TFirst&& first, TOthers&&... others) {
    constexpr std::size_t size = 1 + sizeof...(others);
    return AnyOfThis<typename std::decay<TFirst>::type, size>(std::forward<TFirst>(first),
                                                              std::forward<TOthers>(others)...);
}

template <typename T, int TSize>
bool operator==(const T value, const AnyOfThis<typename std::decay<T>::type, TSize>& anyOfThis) {
    return std::find(anyOfThis.values.begin(), anyOfThis.values.end(), value) != anyOfThis.values.end();
}

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_ANYOF_H_
