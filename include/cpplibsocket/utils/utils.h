#ifndef CPPLIBSOCKET_UTILS_UTILS_H_
#define CPPLIBSOCKET_UTILS_UTILS_H_

#include "cpplibsocket/SocketCommon.h"
#include "cpplibsocket/utils/Optional.h"

namespace cpplibsocket {
namespace utils {

    inline const void* getSinAddr(const Address& address) {
        switch (address.sa_stor.ss_family) {
        case AF_INET:
            return &address.sa_in.sin_addr;
        case AF_INET6:
            return &address.sa_in6.sin6_addr;
        default:
            throw Exception(FUNC_NAME, "Address family ", address.sa_stor.ss_family, " is not supported");
        }
    }

    inline Port getSinPort(const Address& address) {
        switch (address.sa_stor.ss_family) {
        case AF_INET:
            return ntohs(address.sa_in.sin_port);
        case AF_INET6:
            return ntohs(address.sa_in6.sin6_port);
        default:
            throw Exception(FUNC_NAME, "Address family ", address.sa_stor.ss_family, " is not supported");
        }
    }

    inline void setPort(Address& address, const Port port) {
        switch (address.sa_stor.ss_family) {
        case AF_INET:
            address.sa_in.sin_port = htons(port);
            break;
        case AF_INET6:
            address.sa_in6.sin6_port = htons(port);
            break;
        default:
            throw Exception(FUNC_NAME, "Address family ", address.sa_stor.ss_family, " is not supported");
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

    Address getAddressFromFd(SocketHandle socket);

    Endpoint getEndpoint(const Address& address);

    Endpoint getEndpoint(SocketHandle socket);

    Address createAddr(const Endpoint& endpoint);

    Address createAddr(const IPVer ipVersion, const std::string& ipAddress, const Port port);

    static constexpr NullOptionalT UnspecIPVer = NullOptional;

    Optional<Address> resolveHostname(const std::string& hostname,
                                      const Port port,
                                      const Optional<IPVer> ipVersion = UnspecIPVer) noexcept;

    Optional<std::string> resolveHostname(const std::string& hostname,
                                          const Optional<IPVer> ipVersion = UnspecIPVer) noexcept;
} // namespace utils
} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_UTILS_H_
