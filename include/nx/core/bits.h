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

#ifndef INCLUDE_NX_BITS_H_
#define INCLUDE_NX_BITS_H_

#include "nx/core/mpl.h"
#include "nx/core/integer.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

#if defined(NX_TC_GCC)

////
// Forward

/// @brief unsigned long long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned long>>>,  // NOLINT(runtime/int)
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctzll(
      static_cast<unsigned long long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned int>>>,
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctzl(
      static_cast<unsigned long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned int version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned int>>,
unsigned int> BitScanForward(T value) {
  return (value ? __builtin_ctz(
      static_cast<unsigned int>(value))
      : 0);
}

////
// Reverse

/// @brief unsigned long long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned long>>>,  // NOLINT(runtime/int)
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clzll(
      static_cast<unsigned long long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned int>>>,
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clzl(
      static_cast<unsigned long>(value))  // NOLINT(runtime/int)
      : 0);
}

/// @brief unsigned int version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned int>>,
unsigned int> BitScanReverse(T value) {
  return (value ? BitSize<T>::value - 1 - __builtin_clz(
      static_cast<unsigned int>(value))
      : 0);
}

////
// PopCount

/// @brief unsigned long long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned long>>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __builtin_popcountll(
      static_cast<unsigned long long>(value));  // NOLINT(runtime/int)
}

/// @brief unsigned long version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned int>>>,
unsigned int> PopCount(T value) {
  return __builtin_popcountl(
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
}

/// @brief unsigned int version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned int>>,
unsigned int> PopCount(T value) {
  return __builtin_popcount(
      static_cast<unsigned int>(value));
}

#elif defined(NX_TC_VS)
////
// Forward

/// @brief unsigned __int64 version
template <class T>
static NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned __int64>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned long>>>,
unsigned int> BitScanForward(T value) {
  unsigned long index = 0;
  _BitScanForward64(&index,
      static_cast<unsigned __int64>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

/// @brief unsigned long version
template <class T>
static NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long>>,  // NOLINT(runtime/int)
unsigned int> BitScanForward(T value) {
  unsigned long index = 0;
  _BitScanForward(&index,
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

////
// Reverse

/// @brief unsigned __int64 version
template <class T>
static NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned __int64>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned long>>>,
unsigned int> BitScanReverse(T value) {
  unsigned long index = 0;
  _BitScanReverse64(&index,
      static_cast<unsigned __int64>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

/// @brief unsigned long version
template <class T>
static NX_FORCEINLINE EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned long>>,  // NOLINT(runtime/int)
unsigned int> BitScanReverse(T value) {
  unsigned long index = 0;
  _BitScanReverse(&index,
      static_cast<unsigned long>(value));  // NOLINT(runtime/int)
  return static_cast<unsigned int>(index);
}

////
// PopCount

/// @brief unsigned __int64 version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned __int64>,  // NOLINT(runtime/int)
      Not<IntegerFits<T, unsigned int>>>,
unsigned int> PopCount(T value) {
  return static_cast<unsigned int>(__popcnt64(
      static_cast<unsigned __int64>(value)));  // NOLINT(runtime/int)
}

/// @brief unsigned int version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned int>,
      Not<IntegerFits<T, unsigned short>>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __popcnt(
      static_cast<unsigned int>(value));
}

/// @brief unsigned short version
template <class T>
static NX_FORCEINLINE constexpr EnableIf<All<
      std::is_integral<T>,
      IntegerFits<T, unsigned short>>,  // NOLINT(runtime/int)
unsigned int> PopCount(T value) {
  return __popcnt16(
      static_cast<unsigned short>(value));  // NOLINT(runtime/int)
}
#else
#error "Your platform requires an implementation of BSR/BSF."
#endif

template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<value_ && !(value_ & 1)>,
    unsigned int> BitScanForward() {
  return UInt<1 + BitScanForward<
      (static_cast<typename std::make_unsigned<T>::type>(value_) >> 1)>
      >::value;
}
template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<!value_ || value_ & 1>,
    unsigned int> BitScanForward() {
  return 0;
}


template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<
    Bool<value_ && value_ != static_cast<T>(0x1)>,
    unsigned int> BitScanReverse() {
  return UInt<1 + BitScanReverse<
      (static_cast<typename std::make_unsigned<T>::type>(value_) >> 1)>
      >::value;
}
template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<value_ == static_cast<T>(1)>,
    unsigned int> BitScanReverse() {
  return 0;
}
// Give a defined result
template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<!value_>,
    unsigned int> BitScanReverse() {
  return 0;
}

template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<value_ && (value_ & 1)>,
    unsigned int> PopCount() {
  return UInt<1 + PopCount<
      (static_cast<typename std::make_unsigned<T>::type>(value_) >> 1)>
      >::value;
}

template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<value_ && !(value_ & 1)>,
    unsigned int> PopCount() {
  return UInt<PopCount<
      (static_cast<typename std::make_unsigned<T>::type>(value_) >> 1)>
      >::value;
}
template <class T, T value_>
static NX_FORCEINLINE constexpr EnableIf<Bool<!value_>,
    unsigned int> PopCount() {
  return 0;
}

}  // namespace detail
/// @endcond

template <typename T>
class BitUnMasked {
 public:
  // returns 0 for 0
  static NX_FORCEINLINE unsigned int BitScanForward(T value) {
    return detail::BitScanForward(value);
  }
  // returns 0 for 0
  static NX_FORCEINLINE unsigned int BitScanReverse(T value) {
    return detail::BitScanReverse(value);
  }
  static NX_FORCEINLINE unsigned int PopCount(T value) {
    return detail::PopCount(value);
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int BitScanForward() {
    return detail::BitScanForward<T, value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int BitScanReverse() {
    return detail::BitScanReverse<T, value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int PopCount() {
    return detail::PopCount<T, value_>();
  }

};


}  // namespace nx

#endif  // INCLUDE_NX_BITS_H_
