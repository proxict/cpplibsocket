#ifndef CPPLIBSOCKET_UTILS_DEFER_H_
#define CPPLIBSOCKET_UTILS_DEFER_H_

#include <functional>
#include <type_traits>

namespace cpplibsocket {

namespace utils {

    template <typename TFunctorStorage = std::function<void()>>
    class Defer final {
    public:
        template <typename TFunctor>
        explicit Defer(TFunctor functor)
            : mFunctor(std::move(functor)) {
            static_assert(noexcept(std::declval<TFunctor>().operator()()), "The functor has to be noexcept");
        }

        ~Defer() noexcept {
            try {
                if (mFunctor) {
                    mFunctor();
                }
            } catch (...) {
            }
        }

        Defer(Defer&&) = default;

        Defer& operator=(Defer&& other) noexcept {
            mFunctor = std::move(other.mFunctor);
            other.mFunctor = nullptr;
            return *this;
        }

    private:
        Defer(const Defer&) = delete;
        Defer& operator=(const Defer&) = delete;

        TFunctorStorage mFunctor;
    };

    template <typename TFunctor, typename TFunctorStorage = std::function<void()>>
    Defer<typename std::remove_reference<TFunctorStorage>::type> makeDeferred(TFunctor&& functor) {
        return Defer<typename std::remove_reference<TFunctorStorage>::type>(std::forward<TFunctor>(functor));
    }

} // namespace utils

} // namespace cpplibsocket

#endif // CPPLIBSOCKET_UTILS_DEFER_H_
