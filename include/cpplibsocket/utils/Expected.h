#ifndef CPPLIBSOCKET_UTILS_EXPECTED_H_
#define CPPLIBSOCKET_UTILS_EXPECTED_H_

#include "cpplibsocket/common/Assert.h"

#include <memory>
#include <string>
#include <type_traits>

namespace cpplibsocket {

namespace Detail {
    template <typename TOtherError>
    struct ExpectedError {
        using Type = std::decay<TOtherError>;
    };

    template <std::size_t TSize>
    struct ExpectedError<const char (&)[TSize]> {
        using Type = std::string;
    };

    template <std::size_t TSize>
    struct ExpectedError<const wchar_t (&)[TSize]> {
        using Type = std::wstring;
    };
} // namespace Detail

/// Utility meant primarily for return types where an error could be returned instead of the value
///
/// Usage example:
/// Expected<Socket> openSocket() {
///     const int sockFd = ::opensocket(...);
///     if (sockFd == -1) {
///         return makeUnexpected<Socket>("Failed to create a socket");
///     }
///     return Socket(sockFd);
/// }
///
/// Expected<Socket> socket = openSocket();
/// if (!socket) {
///     std::cout << socket.error();
/// } else {
///     socket->listen();
/// }
template <typename T, typename TError = std::string>
class Expected {
public:
    using ValueType = T;

    Expected(const ValueType& value)
        : mValue(value)
        , mInitialized(true) {}

    Expected(ValueType&& value)
        : mValue(std::move(value))
        , mInitialized(true) {}

    Expected(const Expected<ValueType, TError>& other)
        : mInitialized(other.mInitialized) {
        if (mInitialized) {
            new (&mValue) ValueType(other.mValue);
        } else {
            new (mError) TError(other.mError);
        }
    }

    Expected& operator=(const Expected& other) {
        if (mInitialized) {
            mValue.~ValueType();
        } else {
            mError.~TError();
        }
        mInitialized = other.mInitialized;
        if (mInitialized) {
            new (&mValue) ValueType(other.mValue);
        } else {
            new (&mError) TError(other.mError);
        }
        return *this;
    }

    template <typename TOther, typename TOtherError>
    Expected(const Expected<TOther, TOtherError>& other)
        : mInitialized(other.mInitialized) {
        if (mInitialized) {
            new (&mValue) ValueType(other.mValue);
        } else {
            new (&mError) TError(other.mError);
        }
    }

    Expected(Expected<ValueType, TError>&& other)
        : mInitialized(other.mInitialized) {
        if (mInitialized) {
            new (&mValue) ValueType(std::move(other.mValue));
        } else {
            new (&mError) TError(std::move(other.mError));
        }
    }

    ~Expected() {
        if (mInitialized) {
            mValue.~ValueType();
        } else {
            mError.~TError();
        }
    }

    ValueType& value() {
        ASSERT(mInitialized);
        return mValue;
    }

    const ValueType& value() const {
        ASSERT(mInitialized);
        return mValue;
    }

    ValueType* operator->() noexcept {
        ASSERT(mInitialized);
        return &mValue;
    }

    const ValueType* operator->() const noexcept {
        ASSERT(mInitialized);
        return &mValue;
    }

    ValueType& operator*() noexcept {
        ASSERT(mInitialized);
        return mValue;
    }

    const ValueType& operator*() const noexcept {
        ASSERT(mInitialized);
        return mValue;
    }

    const ValueType* operator&() const noexcept {
        ASSERT(mInitialized);
        return &mValue;
    }

    ValueType* operator&() noexcept {
        ASSERT(mInitialized);
        return &mValue;
    }

    const TError& error() const noexcept {
        ASSERT(!mInitialized);
        return mError;
    }

    explicit operator bool() const noexcept { return mInitialized; }

    bool operator!() const noexcept { return !mInitialized; }

private:
    union {
        ValueType mValue;
        TError mError;
    };

    bool mInitialized = false;

    struct Error {};

    template <typename TOtherError>
    Expected(const Error&, TOtherError&& error) {
        new (&mError) TError(std::forward<TOtherError>(error));
    }

    template <typename TValueOther, typename TErrorOther>
    friend class Expected;

    template <typename TOther, typename TOtherError>
    friend Expected<TOther, typename Detail::ExpectedError<TOtherError>::Type>
    makeUnexpected(TOtherError&& error);
};

template <typename T, typename TError>
Expected<T, typename Detail::ExpectedError<TError>::Type> makeUnexpected(TError&& error) {
    using ErrorType = typename Detail::ExpectedError<TError>::Type;
    using ExpectedType = Expected<T, ErrorType>;
    return ExpectedType(typename ExpectedType::Error(), std::forward<ErrorType>(error));
}

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_EXPECTED_H_
