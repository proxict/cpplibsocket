#ifndef CPPLIBSOCKET_UTILS_UTILS_H_
#define CPPLIBSOCKET_UTILS_UTILS_H_

#include "cpplibsocket/SocketCommon.h"

namespace cpplibsocket {

namespace traits {
    template <typename TFrom, typename TTo>
    using CastPointer = typename std::
        conditional<std::is_const<typename std::remove_pointer<TFrom>::type>::value, const TTo*, TTo*>::type;

    template <IPVer TIPVer,
              typename...,
              typename = typename std::enable_if<TIPVer == IPVer::IPV4 || TIPVer == IPVer::IPV6>::type,
              typename T,
              typename TSockAddr =
                  typename std::conditional<TIPVer == IPVer::IPV4, sockaddr_in, sockaddr_in6>::type>
    CastPointer<T, TSockAddr> castAddrPointer(T address) {
        assert(address != nullptr);
        return reinterpret_cast<CastPointer<T, TSockAddr>>(address);
    }
} // namespace traits

namespace utils {

    template <typename..., typename T>
    traits::CastPointer<T, void> getSinAddr(T address) {
        static_assert(std::is_same<typename std::remove_const<typename std::remove_pointer<T>::type>::type,
                                   sockaddr>::value,
                      "address must be of type sockaddr");
        assert(address != nullptr);
        switch (address->sa_family) {
        case AF_INET:
            return &traits::castAddrPointer<IPVer::IPV4>(address)->sin_addr;
        case AF_INET6:
            return &traits::castAddrPointer<IPVer::IPV6>(address)->sin6_addr;
        default:
            throw Exception(FUNC_NAME, "Address family ", address->sa_family, " is not supported");
        }
    }

    template <typename..., typename T>
    typename std::
        conditional<std::is_const<typename std::remove_pointer<T>::type>::value, const Port&, Port&>::type
        getSinPort(T address) {
        static_assert(std::is_same<typename std::remove_const<typename std::remove_pointer<T>::type>::type,
                                   sockaddr>::value,
                      "address must be of type sockaddr");
        assert(address != nullptr);
        switch (address->sa_family) {
        case AF_INET:
            return traits::castAddrPointer<IPVer::IPV4>(address)->sin_port;
        case AF_INET6:
            return traits::castAddrPointer<IPVer::IPV6>(address)->sin6_port;
        default:
            throw Exception(FUNC_NAME, "Address family ", address->sa_family, " is not supported");
        }
    }

    class AddrInfo final {
    public:
        class Iterator {
        public:
            using value_type = struct addrinfo*;
            using difference_type = std::ptrdiff_t;
            using pointer = struct addrinfo*;
            using reference = struct addrinfo*&;
            using iterator_category = std::forward_iterator_tag;

            Iterator(struct addrinfo* info)
                : mInfo(info) {}

            Iterator& operator++() {
                mInfo = mInfo->ai_next;
                return *this;
            }

            Iterator operator++(int) {
                auto i = *this;
                ++(*this);
                return i;
            }

            bool operator==(const Iterator& other) const { return mInfo == other.mInfo; }

            bool operator!=(const Iterator& other) const { return mInfo != other.mInfo; }

            reference operator*() { return mInfo; }

            pointer operator->() { return mInfo; }

        private:
            struct addrinfo* mInfo;
        };

        AddrInfo(const std::string& address, struct addrinfo* hint = nullptr);

        bool empty() const;

        ~AddrInfo() noexcept;

        Iterator begin() const;

        Iterator end() const;

    private:
        struct addrinfo* mInfo;
    };

    sockaddr_storage getAddressFromFd(SocketHandle socket);

    Endpoint getEndpoint(const sockaddr* addr);

    Endpoint getEndpoint(SocketHandle socket);

    sockaddr_storage createAddr(const Endpoint& endpoint);

    sockaddr_storage createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port);

    std::string getLocalIpAddress(const IPVer ipVersion = IPVer::IPV4);

} // namespace utils

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_UTILS_H_
