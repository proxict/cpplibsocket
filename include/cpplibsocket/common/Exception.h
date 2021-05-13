#ifndef CPPLIBSOCKET_COMMON_EXCEPTION_H_
#define CPPLIBSOCKET_COMMON_EXCEPTION_H_

#include <string>
#include <sstream>
#include <stdexcept>

namespace cpplibsocket {

class Exception : public std::runtime_error {
public:
    template <typename ...TArgs>
    explicit Exception(const std::string& message, TArgs&&... args) : std::runtime_error(stringify(message, std::forward<TArgs>(args)...)) {}

    virtual ~Exception() noexcept = default;

    Exception(Exception&&) noexcept = default;

private:
    Exception(const Exception&) = delete;
    Exception& operator=(const Exception&) = delete;
    Exception& operator=(Exception&&) = delete;

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
};

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_COMMON_EXCEPTION_H_
