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

// TODO(nacitar): question use of inlining, add single values to bitmask
#ifndef INCLUDE_NX_CORE_BITS_H_
#define INCLUDE_NX_CORE_BITS_H_

#include "nx/core/mpl.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

template <typename T>
class GenericBits {
 public:
  typedef T type;

  static NX_FORCEINLINE constexpr size_t Size() {
    return ::nx::Size<static_cast<size_t>(sizeof(T)*CHAR_BIT)>::value;
  }
  static NX_FORCEINLINE constexpr bool InRange(
      unsigned int minimum, unsigned int maximum) {
    return (minimum <= Size()  && Size() <= maximum);
  }
  template <unsigned int minimum_, unsigned int maximum_>
  static NX_FORCEINLINE constexpr bool InRange() {
    return Bool<InRange(minimum_, maximum_)>::value;
  }
  template <typename U>
  static NX_FORCEINLINE constexpr bool Fits() {
    return Size() <= GenericBits<U>::Size();
  }
 private:
  NX_UNINSTANTIABLE(GenericBits);
};
template <typename T, class Enable = void>
class Bits : public GenericBits<T> {
 private:
  NX_UNINSTANTIABLE(Bits);
};
template <typename T>
class Bits<T, EnableIf<std::is_integral<T>>> : public GenericBits<T> {
 public:
  static NX_FORCEINLINE constexpr T LowMask(unsigned int length) {
    return ~((~static_cast<T>(0)) << length);
  }
  template <T length_>
  static NX_FORCEINLINE constexpr T LowMask() {
    return ~((~static_cast<T>(0)) << length_);
  }
  static NX_FORCEINLINE constexpr bool MultiplicationOverflow(T kLHS, T kRHS) {
    return (kRHS != 0 && (static_cast<T>(kLHS * kRHS) / kRHS) != kLHS);
  }
  /// @brief Determines if multiplying kLHS with kRHS will result in an
  /// overflow.
  template <T kLHS_, T kRHS_>
  static NX_FORCEINLINE constexpr bool MultiplicationOverflow() {
    return Bool<MultiplicationOverflow(kLHS_, kRHS_)>::value;
  }
  static NX_FORCEINLINE constexpr bool PowerOfTwo(T value) {
    return (value && !(value & (value - 1)));
  }
  template <T value_>
  static NX_FORCEINLINE constexpr bool PowerOfTwo() {
    return Bool<PowerOfTwo(value_)>::value;
  }

 private:
  class Detail {
   public:
    static NX_FORCEINLINE constexpr T Mask() {
      return 0;
    }
    template <typename ... Arguments>
    static NX_FORCEINLINE constexpr T Mask(
        unsigned int index_, Arguments ... rest_) {
      return (static_cast<T>(1u) << index_) | Mask(rest_...);
    }
    template <unsigned int index_, unsigned int ... rest_>
    static NX_FORCEINLINE constexpr T Mask() {
      static_assert((index_ < Bits<T>::Size()), "Bit index out of range.");
      return std::integral_constant<T, Mask(index_, rest_...)>::value;
    }
    template <T value_, unsigned int power_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<
          power_ == 0u>,
        T> Power() {
      return static_cast<T>(1);
    }
    template <T value_, unsigned int power_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<
          power_ == 1u>,
        T> Power() {
      return value_;
    }
    template <T value_, unsigned int power_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<
          power_ != 0u && power_ != 1u>,
        T> Power() {
      static_assert(!MultiplicationOverflow<
          value_, Power<value_, power_ - 1>()>(),
          "Multiplication overflows when computing this exponentiation.");
      return static_cast<T>(value_ * Power<value_, power_ - 1>());
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<
          Bool<!value_ || value_ & static_cast<T>(1)>,
        unsigned int> ScanForward() {
      return 0;
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<
          Bool<value_ && !(value_ & static_cast<T>(1))>,
        unsigned int> ScanForward() {
      typedef typename std::make_unsigned<T>::type unsigned_T;
      return 1 + Bits<unsigned_T>::template ScanForward<
          (static_cast<unsigned_T>(value_) >> 1u)>();
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<!value_>,
        unsigned int> ScanReverse() {
      // Give a defined result
      return 0;
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<value_ == static_cast<T>(1)>,
        unsigned int> ScanReverse() {
      return 0;
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<
        Bool<value_ && value_ != static_cast<T>(1)>,
        unsigned int> ScanReverse() {
      typedef typename std::make_unsigned<T>::type unsigned_T;
      return 1 + Bits<unsigned_T>::template ScanReverse<
          (static_cast<unsigned_T>(value_) >> 1)>();
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<value_ == static_cast<T>(0)>,
        unsigned int> PopCount() {
      return 0;
    }
    template <T value_>
    static NX_FORCEINLINE constexpr EnableIf<Bool<value_ != static_cast<T>(0)>,
        unsigned int> PopCount() {
      typedef typename std::make_unsigned<T>::type unsigned_T;
      return (value_ & 1) + Bits<unsigned_T>::template PopCount<
          (static_cast<unsigned_T>(value_) >> 1)>();
    }
    template <T mask_, T value_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == static_cast<T>(0)>,  // empty
        void> assign(PointerType* data) {
      // empty mask - do nothing
    }
    template <T mask_, T value_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == ~static_cast<T>(0)>,  // full
        void> assign(PointerType* data) {
      // full mask - assign
      *data = value_;
    }
    template <T mask_, T value_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ != static_cast<T>(0) &&  // not empty
            mask_ != ~static_cast<T>(0) &&  // not full
            (mask_ & value_) == mask_>,  // all bits set
        void> assign(PointerType* data) {
      // bit mask, all bits set - or
      *data |= mask_;
    }
    template <T mask_, T value_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ != static_cast<T>(0) &&  // not empty
            mask_ != ~static_cast<T>(0) &&  // not full
            (mask_ & value_) == static_cast<T>(0)>,  // all bits unset
        void> assign(PointerType* data) {
      // bit mask, all bits unset - and inverted mask
      *data &= ~mask_;
    }
    template <T mask_, T value_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ != static_cast<T>(0) &&  // not empty
            mask_ != ~static_cast<T>(0) &&  // not full
            (mask_ & value_) != static_cast<T>(0) &&  // all bits not unset
            (mask_ & value_) != mask_>,  // all bits not set
        void> assign(PointerType* data) {
      // being extra sure that the 'and' is optimized out
      typedef std::integral_constant<T, (value_ & mask_)> masked_value;
      // bit mask, bits not all the same - merge bits
      *data = masked_value::value | (*data & ~mask_);
    }
    template <class PointerType>
    static NX_FORCEINLINE T get(T mask, PointerType* data) {
      // bit mask - return bits
      return (*data & mask);
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == static_cast<T>(0)>,  // empty
        T> get(PointerType* data) {
      // empty mask - return nothing
      return static_cast<T>(0);
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == ~static_cast<T>(0)>,  // full
        T> get(PointerType* data) {
      // full mask - return value
      return *data;
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ != static_cast<T>(0) &&  // not empty
            mask_ != ~static_cast<T>(0)>,  // not full
        T> get(PointerType* data) {
      // bit mask - return bits
      return get<PointerType>(mask_, data);
    }
    template <class PointerType>
    static NX_FORCEINLINE void set(T mask, PointerType* data) {
      *data |= mask;
    }
    template <class PointerType>
    static NX_FORCEINLINE void clear(T mask, PointerType* data) {
      *data &= ~mask;
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE void set(PointerType* data) {
      assign<mask_, ~static_cast<T>(0)>(data);
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE void clear(PointerType* data) {
      assign<mask_, static_cast<T>(0)>(data);
    }
    template <class PointerType>
    static NX_FORCEINLINE void assign(T mask, T value, PointerType* data) {
      // generic bit mask - merge bits
      *data = (value & mask) | (*data & ~mask);
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == static_cast<T>(0)>,  // empty
        void> assign(T value, PointerType* data) {
      // empty mask - do nothing
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ == ~static_cast<T>(0)>,  // full
        void> assign(T value, PointerType* data) {
      // full mask - assign
      *data = value;
    }
    template <T mask_, class PointerType>
    static NX_FORCEINLINE EnableIf<Bool<
            mask_ != static_cast<T>(0) &&  // not empty
            mask_ != ~static_cast<T>(0)>,  // not full
        void> assign(T value, PointerType* data) {
      // bit mask - merge bits
      assign(mask_, value, data);
    }
  };

 public:
  // Mask
  template <typename ... Arguments>
  static NX_FORCEINLINE constexpr T Mask(Arguments... indexes_) {
    return Detail::Mask(indexes_...);
  }
  template <unsigned int ... indexes_>
  static NX_FORCEINLINE constexpr T Mask() {
    return Detail::template Mask<indexes_...>();
  }

  // NOTE: intentionally omitting dynamic power
  template <T value_, unsigned int power_>
  static NX_FORCEINLINE constexpr T Power() {
    return Detail::template Power<value_, power_>();
  }

  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int ScanForward() {
    return Detail::template ScanForward<value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int ScanReverse() {
    return Detail::template ScanReverse<value_>();
  }
  template <T value_>
  static NX_FORCEINLINE constexpr unsigned int PopCount() {
    return Detail::template PopCount<value_>();
  }

  template <class PointerType>
  static NX_FORCEINLINE void assign(T mask, T value, PointerType* data) {
    return Detail::template assign<PointerType>(mask, value, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void assign(T value, PointerType* data) {
    return Detail::template assign<mask_, PointerType>(value, data);
  }
  template <T mask_, T value_, class PointerType>
  static NX_FORCEINLINE void assign(PointerType* data) {
    return Detail::template assign<mask_, value_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE T get(T mask, PointerType* data) {
    return Detail::template get<PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE T get(PointerType* data) {
    return Detail::template get<mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void set(T mask, PointerType* data) {
    Detail::template set<PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void set(PointerType* data) {
    Detail::template set<mask_, PointerType>(data);
  }
  template <class PointerType>
  static NX_FORCEINLINE void clear(T mask, PointerType* data) {
    Detail::template clear<PointerType>(mask, data);
  }
  template <T mask_, class PointerType>
  static NX_FORCEINLINE void clear(PointerType* data) {
    Detail::template clear<mask_, PointerType>(data);
  }
 private:
  NX_UNINSTANTIABLE(Bits);
};

}  // namespace detail
/// @endcond

template <class T>
using Bits = detail::Bits<T>;

}  // namespace nx

#endif  // INCLUDE_NX_CORE_BITS_H_
