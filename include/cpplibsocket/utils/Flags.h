#ifndef UTILS_FLAGS_H
#define UTILS_FLAGS_H

#include <type_traits>

#include <cstdint>

namespace utils {
class EmptyFlagsT final {
public:
    EmptyFlagsT() = delete;

    enum class Construct { Token };

    explicit constexpr EmptyFlagsT(Construct) {}
};

namespace detail {
    template <typename T>
    using IsEnumClass =
        std::integral_constant<bool, std::is_enum<T>::value && !std::is_convertible<T, int>::value>;

    template <bool... T>
    struct StaticAllOf;

    template <>
    struct StaticAllOf<> {
        static constexpr bool value = true;
    };

    template <bool... T>
    struct StaticAllOf<true, T...> {
        static constexpr bool value = StaticAllOf<T...>::value;
    };

    template <bool... T>
    struct StaticAllOf<false, T...> {
        static constexpr bool value = false;
    };
} // namespace detail

static constexpr EmptyFlagsT EmptyFlags(EmptyFlagsT::Construct::Token);

/// This class represents a combination of bit flags of the given enum class type.
///
/// Only works with enum class types. The values in the enum class should form an arithmetic
/// sequence with common difference of 1 {1, 2, 3, ...}.
template <typename TEnum, bool IsEnumClass = detail::IsEnumClass<TEnum>::value>
class Flags;

template <typename TEnum>
class Flags<TEnum, true> final {
public:
    /// Constructs an empty flags object.
    constexpr Flags(EmptyFlagsT = EmptyFlags) noexcept
        : mFlags(0) {}

    /// Constructs this object from all the flags passed into the constructor in an OR manner.
    ///
    /// That means it's equivalent to calling a default constructor and then calling operator|= with
    /// all the given flags.
    ///
    /// \throws ErrorEnumValueTooHigh if any of the given flags exceed the value of 63. If used in a
    ///  constexpr context, the compilation fails with this error.
    template <typename... TFlags>
    constexpr Flags(const TEnum flag, const TFlags... flags)
        : mFlags(construct(flag, flags...)) {}

    /// Tells whether or not the given flag is set.
    /// \param flag The flag to test.
    /// \returns true if the given flag is set, false otherwise.
    /// \throws ErrorEnumValueTooHigh if the \ref flag exceeds value of 63. If used in a constexpr
    ///  context, the compilation fails with this error.
    constexpr bool isSet(const TEnum flag) const { return (mFlags & toFlag(flag)) != 0; }

    /// Tells if the object has no flag set.
    /// \returns true if no flag is set, false otherwise.
    constexpr bool isEmpty() const noexcept { return mFlags == 0; }

    /// Compares flags on the left side to flags on the right side.
    /// \returns true if both sides are equal, false otherwise.
    friend constexpr bool operator==(const Flags& lhs, const Flags& rhs) noexcept {
        return lhs.mFlags == rhs.mFlags;
    }

    /// \copydoc bool operator==(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref rhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref rhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr bool operator==(const Flags& lhs, const TEnum rhs) {
        return lhs.mFlags == lhs.toFlag(rhs);
    }

    /// \copydoc bool operator==(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref lhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref lhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr bool operator==(const TEnum lhs, const Flags& rhs) { return rhs == lhs; }

    /// Compares flags on the left side to flags on the right side.
    /// \returns true if the flags differ, false if they are equal.
    friend constexpr bool operator!=(const Flags& lhs, const Flags& rhs) noexcept { return !(lhs == rhs); }

    /// \copydoc bool operator!=(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref rhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref rhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr bool operator!=(const Flags& lhs, const TEnum rhs) { return !(lhs == rhs); }

    /// \copydoc bool operator!=(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref lhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref lhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr bool operator!=(const TEnum lhs, const Flags& rhs) { return !(lhs == rhs); }

    /// Returns an intersection of flags in \ref lhs and \ref rhs.
    friend constexpr Flags operator&(const Flags& lhs, const Flags& rhs) noexcept {
        return Flags(lhs.mFlags & rhs.mFlags);
    }

    /// \copydoc bool operator&(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref rhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref rhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator&(const Flags& lhs, const TEnum rhs) {
        return Flags(lhs.mFlags & lhs.toFlag(rhs));
    }

    /// \copydoc bool operator&(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref lhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref lhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator&(const TEnum lhs, const Flags& rhs) {
        return Flags(rhs.toFlag(lhs) & rhs.mFlags);
    }

    /// Returns a union of flags in \ref lhs and \ref rhs.
    friend constexpr Flags operator|(const Flags& lhs, const Flags& rhs) noexcept {
        return Flags(lhs.mFlags | rhs.mFlags);
    }

    /// \copydoc bool operator|(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref rhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref rhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator|(const Flags& lhs, const TEnum rhs) {
        return Flags(lhs.mFlags | lhs.toFlag(rhs));
    }

    /// \copydoc bool operator|(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref lhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref lhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator|(const TEnum lhs, const Flags& rhs) {
        return Flags(rhs.toFlag(lhs) | rhs.mFlags);
    }

    /// Returns an exclusive disjunction of flags in \ref lhs and \ref rhs.
    friend constexpr Flags operator^(const Flags& lhs, const Flags& rhs) noexcept {
        return Flags(lhs.mFlags ^ rhs.mFlags);
    }

    /// \copydoc bool operator^(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref rhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref rhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator^(const Flags& lhs, const TEnum rhs) {
        return Flags(lhs.mFlags ^ lhs.toFlag(rhs));
    }

    /// \copydoc bool operator^(const Flags& lhs, const Flags& rhs)
    /// \throws ErrorEnumValueTooHigh if \ref lhs exceeds value of 63 and ErrorEnumNegativeValue
    ///  if \ref lhs is a negative value. If used in a constexpr context, the compilation fails with
    ///  this error.
    friend constexpr Flags operator^(const TEnum lhs, const Flags& rhs) {
        return Flags(rhs.toFlag(lhs) ^ rhs.mFlags);
    }

    /// Returns a complement of flags in this object.
    constexpr Flags operator~() const noexcept { return Flags(~mFlags); }

    /// Adds all the flags that are set in the object on the right side.
    Flags& operator|=(const Flags& flags) noexcept {
        mFlags |= flags.mFlags;
        return *this;
    }

    /// Removes all the flags that are not set in the object on the right side.
    Flags& operator&=(const Flags& flags) noexcept {
        mFlags &= flags.mFlags;
        return *this;
    }

    struct ErrorEnumValueTooHigh {};
    struct ErrorEnumNegativeValue {};

private:
    using FlagsType = std::uint64_t;
    constexpr Flags(const FlagsType flags)
        : mFlags(flags) {}

    using TUnderlying = typename std::underlying_type<TEnum>::type;

    constexpr FlagsType toFlag(const TEnum flag) const {
        return isUnderflowing(flag)
                   ? (throw ErrorEnumNegativeValue())
                   : (isOverflowing(flag) ? (throw ErrorEnumValueTooHigh())
                                          : (FlagsType(1) << static_cast<TUnderlying>(flag)));
    }

    constexpr FlagsType construct(const TEnum flag) const { return toFlag(flag); }

    template <typename... TFlags>
    constexpr FlagsType construct(const TEnum flag, const TFlags... flags) const {
        static_assert(detail::StaticAllOf<std::is_same<TEnum, TFlags>::value...>::value,
                      "All the flags must have the same enum class type");
        return toFlag(flag) | construct(flags...);
    }

    constexpr bool isOverflowing(const TEnum flag) const {
        using TUnderlyingUnsigned = typename std::make_unsigned<TUnderlying>::type;
        return TUnderlyingUnsigned(flag) >= (sizeof(mFlags) * 8);
    }

    // This SFINAE construct bypasses the "always-false" warning when testing if an unsigned type value
    // is negative, which always evaluates to false.
    // In order for the SFINAE to work, T must be a deduced type, hence we can't use TUnderlying directly.
    template <typename T,
              typename std::enable_if<detail::IsEnumClass<T>::value &&
                                          std::is_unsigned<typename std::underlying_type<T>::type>::value,
                                      bool>::type = 0>
    static constexpr bool isUnderflowing(const T) {
        return false;
    }

    template <typename T,
              typename std::enable_if<detail::IsEnumClass<T>::value &&
                                          !std::is_unsigned<typename std::underlying_type<T>::type>::value,
                                      bool>::type = 1>
    static constexpr bool isUnderflowing(const T flag) {
        return typename std::underlying_type<T>::type(flag) < 0;
    }

    FlagsType mFlags;
};

} // namespace utils

/// Creates a union of two given flags.
template <typename TEnum, typename = typename std::enable_if<utils::detail::IsEnumClass<TEnum>::value>::type>
constexpr utils::Flags<TEnum> operator|(const TEnum flag1, const TEnum flag2) {
    return utils::Flags<TEnum>(flag1, flag2);
}

#endif // UTILS_FLAGS_H
