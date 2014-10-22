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

/// @file integer.h
/// @brief Templates and typedefs for handling the selection of appropriate
/// integral types, includind exact-sized types as well as types in certain
/// bit ranges.

#ifndef INCLUDE_NX_CORE_INTEGER_H_
#define INCLUDE_NX_CORE_INTEGER_H_

#ifndef NX_EMBEDDED
#ifndef NX_TARGET_WINDOWS
#include <sys/types.h>  // pid_t
#endif
#endif

#include "nx/core/mpl.h"
#include "nx/core/bits.h"

/// @brief Library namespace.
namespace nx {

/// @cond nx_detail
namespace detail {

template <typename T, typename Preferred, typename = void>
class PreferIntegralTypeInternal : public Identity<T> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralTypeInternal);
};

template <typename T, typename Preferred>
class PreferIntegralTypeInternal<
    T,
    Preferred,
    EnableIf<All<std::is_integral<T>, std::is_integral<Preferred>>>>
    : public std::conditional<
        sizeof(T) == sizeof(Preferred), Preferred, T> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralTypeInternal);
};

template <bool kSigned, typename T, typename = void>
class PreferIntegralSignInternal : public Identity<T> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralSignInternal);
};

template <bool kSigned, typename T>
class PreferIntegralSignInternal<
    kSigned,
    T,
    EnableIf<std::is_integral<T>>>
    : public SetSigned<kSigned, T> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralSignInternal);
};

}  // namespace detail
/// @endcond

/// @brief If kType and kPreferred are integral and of the same size, use the
/// kPreferred type instead of kType.
template <typename T, typename Preferred>
class PreferIntegralType
    : public detail::PreferIntegralTypeInternal<T, Preferred> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralType);
};

/// @brief If kType is integral, set its sign as specified.  Use the provided
/// type otherwise.
template <bool kSigned, typename T>
class PreferIntegralSign
    : public detail::PreferIntegralSignInternal<kSigned, T> {
 private:
  NX_UNINSTANTIABLE(PreferIntegralSign);
};

/// @brief Searches for the smallest signed integral type within the specified
/// bit range.  Provides InvalidType if no such type exists.
template <
    bool kSigned,
    unsigned int kBitMin,
    unsigned int kBitMax = ~0u>
class IntegralLeastRangeSearch
    : public PreferIntegralSign<
        kSigned,
        Invoke<PreferIntegralType<
          Invoke<PreferIntegralType<
            Conditional<
              Bool<Bits<char>::InRange<kBitMin, kBitMax>()>,
              char,
              Conditional<
                Bool<Bits<short>::InRange<  // NOLINT(runtime/int)
                    kBitMin, kBitMax>()>,
                short,  // NOLINT(runtime/int)
                Conditional<
                  Bool<Bits<int>::InRange<kBitMin, kBitMax>()>,
                  int,
                  Conditional<
                    Bool<Bits<long>::InRange<  // NOLINT(runtime/int)
                        kBitMin, kBitMax>()>,
                    long,  // NOLINT(runtime/int)
                    Conditional<
                      Bool<Bits<long long>::InRange<  // NOLINT(runtime/int)
                          kBitMin, kBitMax>()>,
                      long long,  // NOLINT(runtime/int)
                      InvalidType>
                  >
                >
              >
            >,
            // prefer short over char
            short  // NOLINT(runtime/int)
          >>,
          int  // prefer int overall
        >>> {
 private:
  NX_UNINSTANTIABLE(IntegralLeastRangeSearch);
};

/// @brief Provides the smallest integer type in the specified bit range, with
/// a sign if requested.  Prefers short in cases where char is the same size as
/// a short, due to "char" being interpreted by some things as a non-numeric
/// sort of data.  Also, prefers int in all cases where the chosen type is the
/// same size as an int.  Using this structure directly, instead of the aliased
/// versions, means you have to access the type member.
template <
    bool kSigned,
    unsigned int kBitMin,
    unsigned int kBitMax = ~0u>
class IntegralLeastRangeTraits
    : public AssertValidType<
        Invoke<IntegralLeastRangeSearch<kSigned, kBitMin, kBitMax>>,
        int  // fallback type for when our static assert fails
      > {
 private:
  NX_UNINSTANTIABLE(IntegralLeastRangeTraits);
};

// The following types are meant to be used in place of builtin integer types
// and as such their names are violating standards to match the convention seen
// in other libraries of being lowercase with underscores.

/// @brief Provides the smallest integer type signed as requested in the
/// specified bit range.  A static assertion fails if none exists.
template <
    bool kSigned,
    unsigned int kBitMin,
    unsigned int kBitMax = ~0u>
using integral_least_range_t =
    Invoke<IntegralLeastRangeTraits<kSigned, kBitMin, kBitMax>>;

/// @brief Provides the smallest integer type signed as requested in the
/// specified bit range, or InvalidType if none exists.
template <
    bool kSigned,
    unsigned int kBitMin,
    unsigned int kBitMax = ~0u>
using integral_least_range_search_t =
    Invoke<IntegralLeastRangeSearch<kSigned, kBitMin, kBitMax>>;

/// @brief Provides a signed exact-size integer type, if available.
template <unsigned int kBits>
using int_t = integral_least_range_t<true, kBits, kBits>;

/// @brief Provides the smallest signed integer type with at least the
/// specified number of bits.
template <unsigned int kBits>
using int_least_t = integral_least_range_t<true, kBits>;

/// @brief Provides the smallest signed integer type in the specified bit
/// range.
template <unsigned int kBitMin, unsigned int kBitMax>
using int_least_range_t = integral_least_range_t<true, kBitMin, kBitMax>;

/// @brief Provides a unsigned exact-size integer type, if available.
template <unsigned int kBits>
using uint_t = integral_least_range_t<false, kBits, kBits>;

/// @brief Provides the smallest unsigned integer type with at least the
/// specified number of bits.
template <unsigned int kBits>
using uint_least_t = integral_least_range_t<false, kBits>;

/// @brief Provides the smallest signed integer type in the specified bit
/// range.
template <unsigned int kBitMin, unsigned int kBitMax>
using uint_least_range_t = integral_least_range_t<false, kBitMin, kBitMax>;

/// @brief The largest available unsigned integral type.
typedef unsigned long long uintmax_t;  // NOLINT(runtime/int)

/// @brief The largest available signed integral type.
typedef signed long long intmax_t;  // NOLINT(runtime/int)

/// @brief An unsigned integer type 8 bits in size.
typedef uint_t<8>  uint8_t;

/// @brief A signed integer type 8 bits in size.
typedef int_t<8>   int8_t;

/// @brief An unsigned integer type 16 bits in size.
typedef uint_t<16> uint16_t;

/// @brief A signed integer type 16 bits in size.
typedef int_t<16>  int16_t;

/// @brief An unsigned integer type 32 bits in size.
typedef uint_t<32> uint32_t;

/// @brief A signed integer type 32 bits in size.
typedef int_t<32>  int32_t;

/// @brief An unsigned integer type 64 bits in size.
typedef uint_t<64> uint64_t;

/// @brief A signed integer type 64 bits in size.
typedef int_t<64>  int64_t;

/// @brief The smallest unsigned integer type at least 8 bits in size.
typedef uint_least_t<8>  uint_least8_t;

/// @brief The fastest unsigned integer type at least 8 bits in size.
typedef uint_least8_t    uint_fast8_t;

/// @brief The smallest signed integer type at least 8 bits in size.
typedef int_least_t<8>   int_least8_t;

/// @brief The fastest signed integer type at least 8 bits in size.
typedef int_least8_t     int_fast8_t;

/// @brief The smallest unsigned integer type at least 16 bits in size.
typedef uint_least_t<16> uint_least16_t;

/// @brief The fastest unsigned integer type at least 16 bits in size.
typedef uint_least16_t   uint_fast16_t;

/// @brief The smallest signed integer type at least 16 bits in size.
typedef int_least_t<16>  int_least16_t;

/// @brief The fastest unsigned integer type at least 16 bits in size.
typedef int_least16_t    int_fast16_t;

/// @brief The smallest unsigned integer type at least 32 bits in size.
typedef uint_least_t<32> uint_least32_t;

/// @brief The fastest unsigned integer type at least 32 bits in size.
typedef uint_least32_t   uint_fast32_t;

/// @brief The smallest signed integer type at least 32 bits in size.
typedef int_least_t<32>  int_least32_t;

/// @brief The fastest unsigned integer type at least 32 bits in size.
typedef int_least32_t    int_fast32_t;

/// @brief The smallest unsigned integer type at least 64 bits in size.
typedef uint_least_t<64> uint_least64_t;

/// @brief The fastest unsigned integer type at least 64 bits in size.
typedef uint_least64_t   uint_fast64_t;

/// @brief The smallest signed integer type at least 64 bits in size.
typedef int_least_t<64>  int_least64_t;

/// @brief The fastest unsigned integer type at least 64 bits in size.
typedef int_least64_t    int_fast64_t;

/// @brief An unsigned integer type of the same bit size as that of a pointer.
typedef uint_least_t<Bits<void*>::Size()> uintptr_t;

/// @brief A signed integer type of the same bit size as that of a pointer.
typedef int_least_t<Bits<void*>::Size()> intptr_t;

/// @brief A signed integer type able to represent the result of any valid
/// pointer subtraction operation.
typedef std::ptrdiff_t ptrdiff_t;

// TODO: need this typedef over in mpl
/// @brief An unsigned integer type that can store the maximum size of a
/// theoretically possible object of any type (including array).
typedef std::size_t size_t;

typedef std::make_signed<size_t>::type ssize_t;
#ifndef NX_EMBEDDED

#ifdef NX_TARGET_WINDOWS
/// @brief A type capable of holding a process identifier.
typedef DWORD pid_t;
#else
/// @brief A type capable of holding a process identifier.
typedef pid_t pid_t;
#endif

/// @brief An unsigned integer type of the same bit size as that of a pid_t.
typedef uint_least_t<Bits<pid_t>::Size()> uintpid_t;

/// @brief A signed integer type of the same bit size as that of a pid_t.
typedef int_least_t<Bits<pid_t>::Size()> intpid_t;

#endif

}  // namespace nx

#endif  // INCLUDE_NX_CORE_INTEGER_H_
