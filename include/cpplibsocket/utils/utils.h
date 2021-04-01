#ifndef CPPLIBSOCKET_UTILS_UTILS_H_
#define CPPLIBSOCKET_UTILS_UTILS_H_

#include "cpplibsocket/SocketCommon.h"

#include <functional>
#include <type_traits>

namespace cpplibsocket {

namespace utils {

    namespace detail {

        Port getBoundPort(SocketHandle socket);

        std::string getIpAddress(const sockaddr& addr);

        Port getPort(const sockaddr& addr);

        sockaddr createAddr(const IPVer ipVersion, const std::string& hostIp, const Port port);

    } // namespace detail

    template <typename TFunctor = std::function<void()>>
    class Finally final {
    protected:
        TFunctor mFunctor;

    public:
        Finally(TFunctor functor)
            : mFunctor(std::move(functor)) {}

        ~Finally() { mFunctor(); }
    };

    template <typename TFunctor, typename TRefFree = typename std::remove_reference<TFunctor>::type>
    Finally<TRefFree> makeFinally(TFunctor&& closure) {
        return Finally<TRefFree>(std::forward<TFunctor>(closure));
    }

    std::string getHostIpAddress(const std::string& hostName, const IPVer ipVersion = IPVer::IPV4);

    std::string getLocalIpAddress(const IPVer ipVersion = IPVer::IPV4);

    Port getFreePort();

} // namespace utils

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_UTILS_H_
