//
// Copyright (C) 2014 Jacob McIntosh <nacitar at ubercpp dot com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

/// @file mpl.h
/// @brief A collection of metaprogramming tools.  Operations desired only at
/// compilation time are stored directly in the library namespace.  However,
/// operations which may be desired at runtime as well may have
/// implementations for values not known at compilation-time which most
/// naturally would have the same name.  For this reason, the compilation-time
/// versions of these are placed into a sub-namespace "mpl".

#ifndef INCLUDE_NX_CORE_MPL_H_
#define INCLUDE_NX_CORE_MPL_H_

#include "nx/core/os.h"

#ifdef NX_EMBEDDED
  #include "nx/core/embedded/type_traits.h"
#else
  #include <type_traits>
#endif

/// @brief Preprocessor text concatenation.
#define NX_PP_CAT(x, y) NX_PP_CAT1(x, y)

/// @brief Preprocessor text concatenation helper.
#define NX_PP_CAT1(x, y) x##y

/// @brief When placed within the private section of a class/structure this
/// will disallow the default constructor.
#define NX_INCONSTRUCTIBLE(TypeName) \
  TypeName() = delete

/// @brief When placed within the private section of a class/structure this
/// will disallow the move constructor.
#define NX_IMMOVABLE(TypeName) \
  TypeName(TypeName&&) = delete

/// @brief When placed within the private section of a class/structure this
/// will disallow the copy constructor and the assignment operator.
#define NX_NONCOPYABLE(TypeName) \
  TypeName(const TypeName&) = delete;   \
  void operator=(const TypeName&) = delete

/// @brief When placed in the private section of a class/structure, this
/// will disallow the default, move, and copy constructors as well as the
/// assignment operator.
/// @see NX_INCONSTRUCTIBLE
/// @see NX_IMMOVABLE
/// @see NX_NONCOPYABLE
#define NX_UNINSTANTIABLE(TypeName) \
  NX_INCONSTRUCTIBLE(TypeName); \
  NX_IMMOVABLE(TypeName); \
  NX_NONCOPYABLE(TypeName)

// It is suggested that any types that use a static-assertion-using class be
// implemented not as alias templates but rather as actual classes.  That way,
// gcc's error messages are more meaningful.

/// @brief Library namespace.
namespace nx {

/// @brief A function pointer type returning the type Return and accepting the
/// provided argument types.
template <typename Return, typename... Arguments>
using Function = Return (*)(Arguments...);

/// @brief A member function pointer type for a member of Class which returns
/// the type Return and accepting the provided argument types.
template <typename Return, typename Class, typename... Arguments>
using MemberFunction = Return (Class::*)(Arguments...);

/// @brief Simple wrapper around an integral constant.  Can be used to make a
/// value depend upon a template parameter by passing the types as additional
/// arguments to the template.
template<typename T, T kValue, typename...>
class DependentIntegralConstant : public std::integral_constant<T, kValue> {
};

/// @brief A dependent boolean type
template <bool kValue, typename... T>
class DependentBool : public DependentIntegralConstant<bool, kValue, T...> {
};

// No need to invoke integral constants... they resolve to themselves!

/// @brief Meta-constant boolean
template <bool kValue>
class Bool : public std::integral_constant<bool, kValue> {
};

/// @brief Meta-constant int
template <int kValue>
class Int : public std::integral_constant<int, kValue> {
};

/// @brief Meta-constant unsigned int
template <unsigned int kValue>
class UInt : public std::integral_constant<unsigned int, kValue> {
};

/// @brief Meta-constant true value; equivalent to std::true_type
class True : public Bool<true> {
};

/// @brief Meta-constant false value; equivalent to std::false_type
class False : public Bool<false> {
};

/// @brief Basic identity metafunction; provides the type unaltered. Useful for
/// passing raw types to templates expecting a type member.
template <typename T>
class Identity {
 public:
  /// @brief The type provided as a template argument.
  typedef T type;
 private:
  NX_NONCOPYABLE(Identity);
};

/// @brief Alias for removing typename and type members from boilerplate.
template <typename T>
using Invoke = typename T::type;

/// @brief Alias to get the conditional of something with a value member.
template <typename If, typename Then, typename Else>
using Conditional = Invoke<std::conditional<
    static_cast<bool>(If::value), Then, Else>>;

/// @brief Meta-logical negation (Not)
template <typename T>
class Not : public Bool<!T::value> {
};

/// @brief Meta-logical disjunction (Or)
template <typename... T>
class Any : public Bool<false> {
};

/// @brief Specialization for checking the truth of one condition before
/// chaining to check the other conditions.
template <typename Head, typename... Tail>
class Any<Head, Tail...> : public Conditional<Head, Bool<true>, Any<Tail...>> {
};

/// @brief Meta-logical conjunction (And)
template <typename... T>
class All : public Bool<true> {
};

/// @brief Specialization for checking the truth of one condition before
/// chaining to check the other conditions.
template <typename Head, typename... Tail>
class All<Head, Tail...> : public Conditional<
    Head, All<Tail...>, Bool<false>> {
};

/// @brief A version of enable_if that takes a trait and resolves itself
template <typename Condition, typename T = void>
using EnableIf = Invoke<std::enable_if<Condition::value, T>>;

/// @brief A negated version of enable_if that takes a trait and resolves
/// itself.
template <typename Condition, typename T = void>
using DisableIf = Invoke<std::enable_if<Not<Condition>::value, T>>;

/// @brief A distinct "invalid" type, useful for metaprogramming.
class InvalidType {
 private:
  NX_UNINSTANTIABLE(InvalidType);
};

/// @brief Checks if the provided type is valid, and if so provides it.
/// Otherwise providing the Fallback type.  If kAssert is true, a static
/// assertion failure will also occur upon an invalid type.
template <bool kAssert, typename T, typename Fallback = T>
class CheckValidType : public Bool<true> {
 public:
  /// @brief T, because T is not InvalidType
  typedef T type;
};

/// @brief Specialization that fails a static assertion on invalid types.
template <typename Fallback>
class CheckValidType<true, InvalidType, Fallback> : public Bool<false> {
 public:
  /// @brief Fallback, because T is not InvalidType
  typedef Fallback type;
  static_assert(
      DependentBool<false, Fallback>::value,
      "No type exists that fulfills the specified requirements.");
};

/// @brief Specialization that does not fail a static assertion on invalid
/// types.
template <typename Fallback>
class CheckValidType<false, InvalidType, Fallback> : public Bool<false> {
 public:
  /// @brief Fallback, because T is not InvalidType
  typedef Fallback type;
};

/// @brief Shorthand for CheckValidType with static assertions.  Using this
/// will make the presence of assertions more clear to the reader.
template <typename T, typename Fallback = T>
class AssertValidType : public CheckValidType<true, T, Fallback> {
};

/// @brief Shorthand for CheckValidType without static assertions.
template <typename T, typename Fallback = T>
class IsValidType : public CheckValidType<false, T, Fallback> {
};

/// @brief Stores the size of the provided type in bits.
template <typename T>
class BitSize : public UInt<sizeof(T)*CHAR_BIT> {
};

/// @brief Checks if the size of the type T is within the requested range.
template <typename T, unsigned int kMin, unsigned int kMax = ~0u>
class BitRange : public Bool<
    (kMin <= BitSize<T>::value && BitSize<T>::value <= kMax)> {
};

/// @brief Determines if integer type T is <= the size of integer type
/// Destination.
template <typename T, typename Destination>
class IntegerFits
    : public All<
          std::is_integral<T>,
          std::is_integral<Destination>,
          Bool< (BitSize<T>::value <= BitSize<Destination>::value)> > {
};

/// @brief Makes an integral type either signed or unsigned based upon the
/// value of kSigned.
template <bool kSigned, typename T>
class SetSigned : public std::conditional<
    kSigned,
    Invoke<std::make_signed<T>>,
    Invoke<std::make_unsigned<T>>> {
 private:
  NX_UNINSTANTIABLE(SetSigned);
};

/// @brief Determines if multiplying kLHS with kRHS will result in an overflow.
template <class T, T kLHS, T kRHS>
class OverflowMult
    : public Bool<(
        kRHS != 0 && (static_cast<T>(kLHS * kRHS) / kRHS) != kLHS)> {
};

/// @brief mpl namespace
namespace mpl {

/// @brief Returns true if the passed integral value is a power of two.
template <class T, T value_>
struct IsPowerOfTwo : Bool<(value_ && !(value_ & (value_ - 1)))> {
};

/// @cond nx_detail
namespace detail {

template <
    class T, T kBase, unsigned int kPower, class Enable = void>
class Power : public std::integral_constant<
    T, kBase * Power<T, kBase, kPower-1>::value> {
  static_assert(
      !OverflowMult<T, kBase, Power<T, kBase, kPower-1>::value>::value,
      "Value overflows type.");
};

template <class T, T kBase, unsigned int kPower>
class Power<T, kBase, kPower, EnableIf< Bool<kPower == 0>>>
    : public std::integral_constant<T, 1> {
};

}  // namespace detail
/// @endcond

/// @todo When http://gcc.gnu.org/bugzilla/show_bug.cgi?id=58059 is fixed,
/// implement a specialization for handling overflow so you get only one
/// error message for it.
/// @brief Determines kBase to the power of kPower.
template <class T, T kBase, unsigned int kPower>
class Power : public detail::Power<T, kBase, kPower> {
};

/// @cond nx_detail
namespace detail {

template <
    typename T, unsigned int kBits, bool kAllowPartial, class Enable = void>
class LowBitMaskInternal : public std::integral_constant<
    T, (static_cast<T>(1) << kBits)-1> {
};

template <typename T, unsigned int kBits, bool kAllowPartial>
class LowBitMaskInternal<
    T, kBits, kAllowPartial,
    EnableIf<Not<std::is_integral<T>>>> : public UInt<0> {
  static_assert(
      DependentBool<false, T>::value, "The provided type is not integral.");
};

template <typename T, unsigned int kBits, bool kAllowPartial>
class LowBitMaskInternal<
    T, kBits, kAllowPartial,
    EnableIf<All<std::is_integral<T>, Bool<(kBits == BitSize<T>::value)>>>>
    : public std::integral_constant<T, ~static_cast<T>(0)> {
};

// If we allow partial masks, we just max out what bits we have if we can't
// hold them all.
template <typename T, unsigned int kBits>
class LowBitMaskInternal<
    T, kBits, true,
    EnableIf<All<std::is_integral<T>, Bool<(kBits > BitSize<T>::value)>>>>
    : public std::integral_constant<T, ~static_cast<T>(0)> {
};

// If we don't allow partial masks, we fail a static assert if we can't hold
// all the bits.
template <typename T, unsigned int kBits>
class LowBitMaskInternal<
    T, kBits, false,
    EnableIf<All<std::is_integral<T>, Bool<(kBits > BitSize<T>::value)>>>>
    : public std::integral_constant<T, 0> {
  static_assert(
      DependentBool<false, T>::value,
      "This type does not have enough bits to hold a mask of this size.");
};

}  // namespace detail
/// @endcond

/// @brief Provides a bit mask of type T with the lowest kBits bits set.
template <typename T, unsigned int kBits, bool kAllowPartial = false>
class LowBitMask : public detail::LowBitMaskInternal<T, kBits, kAllowPartial> {
};

/// @cond nx_detail
namespace detail {

template <typename Type, Type value_, class Enable = void>
struct BitScanForward {
  static constexpr const unsigned int value =
      1 + BitScanForward<Type, (value_ >> 1)>::value;
};

template <typename Type, Type value_>
struct BitScanForward<Type, value_, EnableIf<Bool<value_ & 0x1>>> {
  static constexpr const unsigned int value = 0;
};

// Negative
template <typename Type, Type value_>
struct BitScanForward<Type, value_, EnableIf<Bool<(value_ < 0)>>> {
  static constexpr const unsigned int value =
      BitScanForward<typename std::make_unsigned<Type>::type,
          static_cast<typename std::make_unsigned<Type>::type>(value_)>::value;
};
// The value '0' makes no sense for this operation.
template <typename Type, Type value_>
struct BitScanForward<Type, value_, EnableIf<Bool<value_ == 0>>> {
  static constexpr const unsigned int value = 0;  // defined to reduce errors
  static_assert(sizeof(Type) < 0, "Argh.");
};

}  // namespace detail
/// @endcond

template <typename Type, Type value_>
struct BitScanForward : public detail::BitScanForward<Type, value_> {
};

/// @cond nx_detail
namespace detail {

template <typename Type, Type value, class Enable = void>
struct LowestBitRun {
  static constexpr const Type offset =
      BitScanForward<Type, value>::value;
  static constexpr const Type length =
      BitScanForward<Type, (~(value >> offset))>::value;
};
template <typename Type, Type value>
struct LowestBitRun<Type, value, EnableIf<Bool<value == 0>>> {
  static constexpr const Type offset = 0;
  static constexpr const Type length = 0;
};

}  // namespace detail
/// @endcond

template <typename Type, Type value>
struct LowestBitRun : public detail::LowestBitRun<Type, value> {
};

}  // namespace mpl

/// @cond nx_detail
namespace detail {

template <typename Type, Type mask_, Type bits_>
class BitValueBase {
 public:
  typedef Type value_type;
  static constexpr const value_type mask = mask_;
  static constexpr const value_type bits = bits_;
  static_assert((bits_ & mask_) == bits_, "Unreferenced set bits in value.");
};

// multiple set bits, but not all
template <typename Type, Type mask_, Type bits_, class Enable = void>
class BitValue : public BitValueBase<Type, mask_, bits_> {
 public:
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    *data = bits_ | (*data & ~mask_);
  }
};

// no bits in mask
template <typename Type, Type mask_, Type bits_>
class BitValue<Type, mask_, bits_, EnableIf<Bool<mask_ == 0>>>
    : public BitValueBase<Type, mask_, bits_> {
 public:
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    /* NOOP */
  }
};
// all bits in mask
template <typename Type, Type mask_, Type bits_>
class BitValue<Type, mask_, bits_, EnableIf<Bool<~mask_ == 0>>>
    : public BitValueBase<Type, mask_, bits_> {
 public:
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    *data = bits_;
  }
};
// single set bit
template <typename Type, Type mask_, Type bits_>
class BitValue<Type, mask_, bits_,
    EnableIf<Bool<mpl::IsPowerOfTwo<Type, mask_>::value && bits_ != 0>>>
    : public BitValueBase<Type, mask_, bits_> {
 public:
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    *data |= mask_;
  }
};
// single unset bit
template <typename Type, Type mask_, Type bits_>
class BitValue<Type, mask_, bits_,
    EnableIf<Bool<mpl::IsPowerOfTwo<Type, mask_>::value && bits_ == 0>>>
    : public BitValueBase<Type, mask_, bits_> {
 public:
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    *data &= ~mask_;
  }
};

template <typename Type, Type mask_, Type value_, class Enable = void>
class BitField
    : public BitValue<Type, mask_,
        ((value_ &
            mpl::LowBitMask<
                Type, mpl::LowestBitRun<Type, mask_>::length>::value)
            << mpl::LowestBitRun<Type, mask_>::offset) |
        BitField<Type, (mask_ & ~(
            mpl::LowBitMask<
                Type, mpl::LowestBitRun<Type, mask_>::length>::value
            << mpl::LowestBitRun<Type, mask_>::offset)),
            (value_ >> mpl::LowestBitRun<Type, mask_>::length)>::bits> {
};
template <typename Type, Type mask_, Type value_>
class BitField<Type, mask_, value_, EnableIf<Bool<mask_ == 0>>>
    : public BitValue<Type, 0, 0> {
  // slightly different check than the one in BitValue
  static_assert(value_ == 0, "Extra unused set bits in value.");
};

}  // namespace detail
/// @endcond

template <typename Type, Type mask_, Type value_>
class BitValue : public detail::BitValue<Type, mask_, value_> {
};
template <typename Type, Type mask_, Type value_>
class BitField : public detail::BitField<Type, mask_, value_> {
};
template <typename Mask, typename ... Masks>
class BitTransaction
    : public BitValue<typename Mask::value_type,
        Mask::mask | BitTransaction<Masks...>::mask,
        Mask::bits | BitTransaction<Masks...>::bits> {
  static_assert(std::is_same<
      typename Mask::value_type,
      typename BitTransaction<Masks...>::value_type>::value,
      "All mask types must be the same.");
  static_assert((Mask::mask & BitTransaction<Masks...>::mask) == 0,
      "Masks are not allowed to refer to the same bits; no overlapping.");
};
template <typename Mask>
class BitTransaction<Mask>
    : public BitValue<typename Mask::value_type, Mask::mask, Mask::bits> {
};

}  // namespace nx

#endif  // INCLUDE_NX_CORE_MPL_H_
