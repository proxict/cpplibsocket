#ifndef CPPLIBSOCKET_COMMON_EXCEPTION_H_
#define CPPLIBSOCKET_COMMON_EXCEPTION_H_

#include <string>
#include <sstream>

namespace cpplibsocket {

class Exception {
public:
    template <typename ...TArgs>
    explicit Exception(const std::string& message, TArgs&&... args) {
        mMessage = stringify(message, std::forward<TArgs>(args)...);
    }

    const std::string& what() const noexcept {
        return mMessage;
    }

private:
	template <typename T>
	std::string stringify(const T& arg) {
		std::ostringstream ss;
		ss << arg;
		return ss.str();
	}

	template <typename T, typename ...Args>
	std::string stringify(const T& first, Args&& ...args) {
		return stringify(first) + stringify(std::forward<Args>(args)...);
	}

    std::string mMessage;
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_COMMON_EXCEPTION_H_
