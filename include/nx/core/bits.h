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

/// @file bits.h
/// @brief Provides wrappers for bitwise operations.

// TODO: question use of inlining
#ifndef INCLUDE_NX_BITS_H_
#define INCLUDE_NX_BITS_H_

#include "nx/core/mpl.h"

/// @brief Library namespace.
namespace nx {

/// @brief Bits namespace.
namespace bits {

template <typename T>
NX_FORCEINLINE constexpr unsigned int Size() {
  return UInt<static_cast<unsigned int>(sizeof(T)*CHAR_BIT)>::value;
}
template <typename T>
NX_FORCEINLINE constexpr bool InRange(unsigned int min, unsigned int max) {
  return (min <= Size<T>()  && Size<T>() <= max);
}
template <typename T, unsigned int min_, unsigned int max_>
NX_FORCEINLINE constexpr bool InRange() {
  return InRange<T>(min_,max_);
}
template <typename T, typename U>
NX_FORCEINLINE constexpr bool Fits() {
  return Size<T>() <= Size<U>();
}

template <typename T>
NX_FORCEINLINE constexpr bool MultiplicationOverflow(T kLHS, T kRHS) {
  return (kRHS != 0 && (static_cast<T>(kLHS * kRHS) / kRHS) != kLHS);
}
/// @brief Determines if multiplying kLHS with kRHS will result in an overflow.
template <typename T, T kLHS_, T kRHS_>
NX_FORCEINLINE constexpr bool MultiplicationOverflow() {
  return Bool<MultiplicationOverflow(kLHS_,kRHS_)>::value;
}

template <typename T>
NX_FORCEINLINE constexpr bool PowerOfTwo(T value) {
  return (value && !(value & (value - 1)));
}
template <typename T, T value_>
NX_FORCEINLINE constexpr bool PowerOfTwo() {
  return Bool<PowerOfTwo(value_)>::value;
}


/// @cond nx_detail
namespace detail {

////
// Dynamic implementations

#if defined(NX_TC_GCC) || defined (NX_TC_CLANG)

// Forward

/// @brief unsigned long long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned long>()>>>,  // NOLINT(runtime/int)
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctzll(
      static_cast<unsigned long long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned int>()>>>,
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctzl(
      static_cast<unsigned long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned int version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned int>()>>,
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctz(
      static_cast<unsigned int>(value))
      : 0);
}

// Reverse

/// @brief unsigned long long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned long>()>>>,  // NOLINT(runtime/int)
unsigned int> BitScanReverse(T value) {
  return (value ? bits::Size<T>() - 1 - __builtin_clzll(
      static_cast<unsigned long long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned int>()>>>,
unsigned int> BitScanReverse(T value) {
  return (value ? bits::Size<T>() - 1 - __builtin_clzl(
      static_cast<unsigned long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned int version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned int>()>>,
unsigned int> BitScanReverse(T value) {
  return (value ? bits::Size<T>() - 1 - __builtin_clz(
      static_cast<unsigned int>(value))
      : 0);
}

// PopCount

/// @brief unsigned long long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned long>()>>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __builtin_popcountll(
      static_cast<unsigned long long>(value));  // NOLINT(runtime/int)
}

/// @brief unsigned long version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned int>()>>>,
unsigned int> PopCount(T value) {
  return __builtin_popcountl(
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
}

/// @brief unsigned int version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned int>()>>,
unsigned int> PopCount(T value) {
  return __builtin_popcount(
      static_cast<unsigned int>(value));
}

#elif defined(NX_TC_VS)

// Forward

/// @brief unsigned __int64 version
template <class T>
NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned __int64>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned long>()>>>,
unsigned int> BitScanForward(T value) {
  unsigned long index = 0;
  _BitScanForward64(&index,
      static_cast<unsigned __int64>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

/// @brief unsigned long version
template <class T>
NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long>()>>,  // NOLINT(runtime/int)
unsigned int> BitScanForward(T value) {
  unsigned long index = 0;
  _BitScanForward(&index,
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

// Reverse

/// @brief unsigned __int64 version
template <class T>
NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned __int64>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned long>()>>>,
unsigned int> BitScanReverse(T value) {
  unsigned long index = 0;
  _BitScanReverse64(&index,
      static_cast<unsigned __int64>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

/// @brief unsigned long version
template <class T>
NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned long>()>>,  // NOLINT(runtime/int)
unsigned int> BitScanReverse(T value) {
  unsigned long index = 0;
  _BitScanReverse(&index,
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

// PopCount

/// @brief unsigned __int64 version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned __int64>()>,  // NOLINT(runtime/int)
      Not<Bool<Fits<T, unsigned int>()>>>,
unsigned int> PopCount(T value) {
  return static_cast<unsigned int>(__popcnt64(
      static_cast<unsigned __int64>(value)));  // NOLINT(runtime/int)
}

/// @brief unsigned int version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned int>()>,
      Not<Bool<Fits<T, unsigned short>()>>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __popcnt(
      static_cast<unsigned int>(value));
}

/// @brief unsigned short version
template <class T>
NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      Bool<Fits<T, unsigned short>()>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __popcnt16(
      static_cast<unsigned short>(value));  // NOLINT(runtime/int)
}

#else

#error "Your platform requires an implementation of BSR/BSF."
// to reduce errors, define dummies
template <class T> unsigned int BitScanForward(T value) { return 0; }
template <class T> unsigned int BitScanReverse(T value) { return 0; }
template <class T> unsigned int PopCount(T value) { return 0; }

#endif

////
// Static implementations

template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<!value_ || value_ & static_cast<T>(1)>,
    unsigned int> BitScanForward() {
  return 0;
}
template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<value_ && !(value_ & static_cast<T>(1))>,
    unsigned int> BitScanForward() {
  typedef typename std::make_unsigned<T>::type unsigned_T;
  return 1 +
      BitScanForward<unsigned_T, (static_cast<unsigned_T>(value_) >> 1)>();
}

// Give a defined result
template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<!value_>,
    unsigned int> BitScanReverse() {
  return 0;
}
template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<value_ == static_cast<T>(1)>,
    unsigned int> BitScanReverse() {
  return 0;
}
template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<
    Bool<value_ && value_ != static_cast<T>(1)>,
    unsigned int> BitScanReverse() {
  typedef typename std::make_unsigned<T>::type unsigned_T;
  return 1 +
      BitScanReverse<unsigned_T, (static_cast<unsigned_T>(value_) >> 1)>();
}

template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<value_ == static_cast<T>(0)>,
    unsigned int> PopCount() {
  return 0;
}
template <class T, T value_>
NX_FORCEINLINE constexpr EnableIf<Bool<value_ != static_cast<T>(0)>,
    unsigned int> PopCount() {
  typedef typename std::make_unsigned<T>::type unsigned_T;
  return (value_ & 1) +
      PopCount<unsigned_T, (static_cast<unsigned_T>(value_) >> 1)>();
}


template <class T>
NX_FORCEINLINE constexpr T Mask() {
  return 0;
}
template <class T, typename ... Arguments>
NX_FORCEINLINE constexpr T Mask(unsigned int index_, Arguments ... rest_) {
  return (static_cast<T>(1u) << index_) | Mask<T>(rest_...);
}
template <class T,unsigned int index_, unsigned int ... rest_>
NX_FORCEINLINE constexpr T Mask() {
  static_assert((index_ < bits::Size<T>()), "Bit index out of range.");
  return std::integral_constant<T,Mask<T>(index_, rest_...)>::value;
}



////
// ASSIGN
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == static_cast<T>(0)>, // empty
    void> assign(PointerType* data) {
  // empty mask - do nothing
}
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == ~static_cast<T>(0)>, // full
    void> assign(PointerType* data) {
  // full mask - assign
  *data = value_;
}
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ != static_cast<T>(0) && // not empty
        mask_ != ~static_cast<T>(0) && // not full
        (mask_ & value_) == mask_>, // all bits set
    void> assign(PointerType* data) {
  // bit mask, all bits set - or
  *data |= mask_;
}
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ != static_cast<T>(0) && // not empty
        mask_ != ~static_cast<T>(0) && // not full
        (mask_ & value_) == static_cast<T>(0)>, // all bits unset
    void> assign(PointerType* data) {
  // bit mask, all bits unset - and inverted mask
  *data &= ~mask_;
}
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ != static_cast<T>(0) && // not empty
        mask_ != ~static_cast<T>(0) && // not full
        (mask_ & value_) != static_cast<T>(0) && // all bits not unset
        (mask_ & value_) != mask_>, // all bits not set
    void> assign(PointerType* data) {
  // being extra sure that the 'and' is optimized out
  typedef std::integral_constant<T,(value_ & mask_)> masked_value;
  // bit mask, bits not all the same - merge bits
  *data = masked_value::value | (*data & ~mask_);
}


////
// GET
template <class T, class PointerType>
NX_FORCEINLINE T get(T mask, PointerType* data) {
  // bit mask - return bits
  return (*data & mask);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == static_cast<T>(0)>, // empty
    T> get(PointerType* data) {
  // empty mask - return nothing
  return static_cast<T>(0);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == ~static_cast<T>(0)>, // full
    T> get(PointerType* data) {
  // full mask - return value
  return *data;
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ != static_cast<T>(0) && // not empty
        mask_ != ~static_cast<T>(0)>, // not full
    T> get(PointerType* data) {
  // bit mask - return bits
  return get<T, PointerType>(mask_, data);
}


////
// SET/CLEAR OPERATIONS
template <class T, class PointerType>
NX_FORCEINLINE void set(T mask, PointerType* data) {
  *data |= mask;
}
template <class T, class PointerType>
NX_FORCEINLINE void clear(T mask, PointerType* data) {
  *data &= ~mask;
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE void set(PointerType* data) {
  assign<T, mask_, ~static_cast<T>(0)>(data);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE void clear(PointerType* data) {
  assign<T, mask_, static_cast<T>(0)>(data);
}

////
// GENERIC
template <class T, class PointerType>
NX_FORCEINLINE void assign(T mask, T value, PointerType* data) {
  // generic bit mask - merge bits
  *data = (value & mask) | (*data & ~mask);
}

////
// DYNAMIC
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == static_cast<T>(0)>, // empty
    void> assign(T value, PointerType* data) {
  // empty mask - do nothing
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ == ~static_cast<T>(0)>, // full
    void> assign(T value, PointerType* data) {
  // full mask - assign
  *data = value;
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE EnableIf<Bool<
        mask_ != static_cast<T>(0) && // not empty
        mask_ != ~static_cast<T>(0)>, // not full
    void> assign(T value, PointerType* data) {
  // bit mask - merge bits
  assign<T>(mask_, value, data);
}


template <class T, T value_, unsigned int power_>
NX_FORCEINLINE constexpr EnableIf<Bool<
      power_ == 0u>,
    T> Power() {
  return static_cast<T>(1);
}
template <class T, T value_, unsigned int power_>
NX_FORCEINLINE constexpr EnableIf<Bool<
      power_ == 1u>,
    T> Power() {
  return value_;
}
template <class T, T value_, unsigned int power_>
NX_FORCEINLINE constexpr EnableIf<Bool<
      power_ != 0u && power_ != 1u>,
    T> Power() {
  static_assert(!MultiplicationOverflow<T,
      value_, Power<T, value_, power_ - 1>()>(),
      "Multiplication overflows when computing this exponentiation.");
  return value_ * Power<T, value_, power_ - 1>();
}

}  // namespace detail
/// @endcond

template <class T, T value_, unsigned int power_>
NX_FORCEINLINE constexpr T Power() {
  return detail::Power<T, value_, power_>();
}
// returns 0 for 0
template <typename T>
NX_FORCEINLINE unsigned int ScanForward(T value) {
  return detail::BitScanForward(value);
}
// returns 0 for 0
template <typename T>
NX_FORCEINLINE unsigned int ScanReverse(T value) {
  return detail::BitScanReverse(value);
}
template <typename T>
NX_FORCEINLINE unsigned int PopCount(T value) {
  return detail::PopCount(value);
}
template <typename T, T value_>
NX_FORCEINLINE constexpr unsigned int ScanForward() {
  return detail::BitScanForward<T, value_>();
}
template <typename T, T value_>
NX_FORCEINLINE constexpr unsigned int ScanReverse() {
  return detail::BitScanReverse<T, value_>();
}
template <typename T, T value_>
NX_FORCEINLINE constexpr unsigned int PopCount() {
  return detail::PopCount<T, value_>();
}

template <class T, unsigned int ... indexes_>
NX_FORCEINLINE constexpr T Mask() {
  return detail::Mask<T,indexes_...>();
}

template <class T, typename ... Arguments>
NX_FORCEINLINE constexpr T Mask(Arguments... indexes_) {
  return detail::Mask<T>(indexes_...);
}


template <class T, class PointerType>
NX_FORCEINLINE void assign(T mask, T value, PointerType* data) {
  detail::assign<T, PointerType>(mask, value, data);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE void assign(T value, PointerType* data) {
  detail::assign<T, mask_, PointerType>(value, data);
}
template <class T, T mask_, T value_, class PointerType>
NX_FORCEINLINE void assign(PointerType* data) {
  detail::assign<T, mask_, value_, PointerType>(data);
}
template <class T, class PointerType>
NX_FORCEINLINE T get(T mask, PointerType* data) {
  return detail::get<T, PointerType>(mask, data);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE T get(PointerType* data) {
  return detail::get<T, mask_, PointerType>(data);
}
template <class T, class PointerType>
NX_FORCEINLINE void set(T mask, PointerType* data) {
  detail::set<T, PointerType>(mask, data);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE void set(PointerType* data) {
  detail::set<T, mask_, PointerType>(data);
}
template <class T, class PointerType>
NX_FORCEINLINE void clear(T mask, PointerType* data) {
  detail::clear<T, PointerType>(mask, data);
}
template <class T, T mask_, class PointerType>
NX_FORCEINLINE void clear(PointerType* data) {
  detail::clear<T, mask_, PointerType>(data);
}

}  // namespace bits

// TODO: make only work for integral values
// TODO: add single values to bitmask
// BitRange/IntegerFits
// LowBitMask
template <typename T>
class Bits {
 public:
  typedef T type;
  // returns 0 for 0
  static NX_FORCEINLINE unsigned int ScanForward(T value) {
    return bits::ScanForward<T>(value);
  }
  // returns 0 for 0
  static NX_FORCEINLINE unsigned int ScanReverse(T value) {
    return bits::ScanReverse<T>(value);
  }
  static NX_FORCEINLINE unsigned int PopCount(T value) {
    return bits::PopCount<T>(value);
  }
  static NX_FORCEINLINE constexpr bool MultiplicationOverflow(T kLHS, T kRHS) {
    return bits::MultiplicationOverflow<T>(kLHS,kRHS);
  }
  static NX_FORCEINLINE constexpr bool PowerOfTwo(T value) {
    return bits::PowerOfTwo<T>(value);
  }
  template <typename ... Arguments>
  static NX_FORCEINLINE constexpr T Mask(Arguments... indexes_) {
    return bits::Mask<T>(indexes_...);
  }


  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int ScanForward() {
    return bits::ScanForward<T, value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int ScanReverse() {
    return bits::ScanReverse<T, value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int PopCount() {
    return bits::PopCount<T, value_>();
  }
  template <T kLHS_, T kRHS_>
  static NX_FORCEINLINE constexpr bool MultiplicationOverflow() {
    return bits::MultiplicationOverflow<T, kLHS_, kRHS_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr bool PowerOfTwo() {
    return bits::PowerOfTwo<T, value_>();
  }
  template <unsigned int ... indexes_>
  static NX_FORCEINLINE constexpr T Mask() {
    return bits::Mask<T,indexes_...>();
  }


  static NX_FORCEINLINE constexpr unsigned int Size() {
    return bits::Size<T>();
  }
  template <unsigned int min_, unsigned int max_>
  static NX_FORCEINLINE constexpr bool InRange() {
    return bits::InRange<T, min_, max_>();
  }
  static NX_FORCEINLINE constexpr bool InRange(
      unsigned int min, unsigned int max) {
    return bits::InRange<T>(min, max);
  }
  // NOTE: intentionally omitting dynamic power
  template <T value_, unsigned int power_>
  static NX_FORCEINLINE constexpr T Power() {
    return bits::Power<T, value_, power_>();
  }

  template <class PointerType>
  static NX_FORCEINLINE void assign(T mask, T value, PointerType* data) {
    return bits::assign<T, PointerType>(mask, value, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void assign(T value, PointerType* data) {
    return bits::assign<T, mask_, PointerType>(value, data);
  }
  template <T mask_, T value_, class PointerType>
  static NX_FORCEINLINE void assign(PointerType* data) {
    return bits::assign<T, mask_, value_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE T get(T mask, PointerType* data) {
    return bits::get<T, PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE T get(PointerType* data) {
    return bits::get<T, mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void set(T mask, PointerType* data) {
    bits::set<T, PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    bits::set<T, mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void clear(T mask, PointerType* data) {
    bits::clear<T, PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void clear(PointerType* data) {
    bits::clear<T, mask_, PointerType>(data);
  }
 private:
  NX_UNINSTANTIABLE(Bits);
};


// TODO: toField, fromField
template <class T, T mask_>
class BitMask {
 public:
  typedef T type;
  static constexpr const T mask = mask_;

  template <class PointerType>
  static NX_FORCEINLINE void assign(T value, PointerType* data) {
    return bits::assign<T, mask_, PointerType>(value, data);
  }
  template <T value_, class PointerType>
  static NX_FORCEINLINE void assign(PointerType* data) {
    return bits::assign<T, mask_, value_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE T get(PointerType* data) {
    return bits::get<T, mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    bits::set<T, mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void clear(PointerType* data) {
    bits::clear<T, mask_, PointerType>(data);
  }

 private:
  NX_UNINSTANTIABLE(BitMask);
};
}  // namespace nx

#endif  // INCLUDE_NX_BITS_H_
