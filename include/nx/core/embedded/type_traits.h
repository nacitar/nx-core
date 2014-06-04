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

/// @file type_traits.h
/// @brief Implementation of a large part of type_traits, useful for
/// metaprogramming on embedded platforms which do not ship with C++ libraries
/// due to the faulty rationale of C programmers writing off useful libraries
/// as unnecessary.

#ifndef INCLUDE_NX_CORE_EMBEDDED_TYPE_TRAITS_H_
#define INCLUDE_NX_CORE_EMBEDDED_TYPE_TRAITS_H_

#include <stddef.h>  // size_t

namespace std {

typedef decltype(nullptr) nullptr_t;
typedef decltype(static_cast<char*>(nullptr) - static_cast<char*>(nullptr))
    ptrdiff_t;
typedef size_t size_t;

template<class T, T v>
struct integral_constant {
    static constexpr T value = v;
    typedef T value_type;
    typedef integral_constant type;
    constexpr NX_FORCEINLINE operator value_type() const noexcept {
      return v;
    }
    constexpr NX_FORCEINLINE value_type operator()() const noexcept {
      return v;
    }
};
typedef integral_constant<bool, true>  true_type;
typedef integral_constant<bool, false> false_type;

template<bool B, class T = void> struct enable_if { };
template<class T> struct enable_if<true, T> { typedef T type; };

template<bool B, class T, class F>
struct conditional { typedef T type; };
template<class T, class F>
struct conditional<false, T, F> { typedef F type; };

template< class T > struct remove_reference      {typedef T type;};
template< class T > struct remove_reference<T&>  {typedef T type;};
template< class T > struct remove_reference<T&&> {typedef T type;};
template<class T> struct remove_pointer     { typedef T type; };
template<class T> struct remove_pointer<T*> { typedef T type; };
template<class T> struct remove_volatile             { typedef T type; };
template<class T> struct remove_volatile<volatile T> { typedef T type; };
template<class T> struct remove_const          { typedef T type; };
template<class T> struct remove_const<const T> { typedef T type; };
template<class T> struct remove_cv {
  typedef typename remove_volatile<typename remove_const<T>::type>::type type;
};

template< class T> struct add_const { typedef const T type; };
template< class T> struct add_volatile { typedef volatile T type; };
template< class T >
struct add_cv {
    typedef typename add_volatile<typename add_const<T>::type>::type type;
};
template< class T >
struct add_pointer {
    typedef typename remove_reference<T>::type* type;
};


template<class T, class U>
struct is_same : false_type {};
template<class T>
struct is_same<T, T> : true_type {};

template< class T >
struct is_void : integral_constant<bool,
    is_same<void, typename remove_cv<T>::type>::value> {
};

template <class T>
struct is_null_pointer : is_same<typename remove_cv<T>::type, nullptr_t> {
};

template<class T>
struct is_array : false_type {};
template<class T>
struct is_array<T[]> : true_type {};
template<class T, size_t N>
struct is_array<T[N]> : true_type {};

namespace detail {
  template <class T>
  struct is_integral_helper
      : false_type {
  };
  template <>
  struct is_integral_helper<char>
      : true_type {
  };
  template <>
  struct is_integral_helper<char16_t>
      : true_type {
  };
  template <>
  struct is_integral_helper<char32_t>
      : true_type {
  };
  template <>
  struct is_integral_helper<signed char>
      : true_type {
  };
  template <>
  struct is_integral_helper<unsigned char>
      : true_type {
  };
  template <>
  struct is_integral_helper<signed short>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<unsigned short>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<signed int>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<unsigned int>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<signed long>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<unsigned long>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<signed long long>  // NOLINT(runtime/int)
      : true_type {
  };
  template <>
  struct is_integral_helper<unsigned long long>  // NOLINT(runtime/int)
      : true_type {
  };
}  // namespace detail
template <class T> struct is_integral
    : detail::is_integral_helper<typename remove_cv<T>::type> {
};

namespace detail {
  template <class T, class Enable = void>
  struct is_signed_helper : false_type {
  };
  template <class T>
  struct is_signed_helper<T,
      typename enable_if<(is_integral<T>::value)>::type>
      : integral_constant<bool, (static_cast<T>(-1) < 0)> {
  };
  template <class T, class Enable = void>
  struct is_unsigned_helper : false_type {
  };
  template <class T>
  struct is_unsigned_helper<T,
      typename enable_if<(is_integral<T>::value)>::type>
      : integral_constant<bool, (static_cast<T>(-1) >= 0)> {
  };
}  // namespace detail

template <class T> struct is_signed
    : detail::is_signed_helper<typename remove_cv<T>::type> {
};
template <class T> struct is_unsigned
    : detail::is_unsigned_helper<typename remove_cv<T>::type> {
};
namespace detail {
  template <class T, class Enable = void>
  struct make_unsigned;

  template <class T> struct make_unsigned<T,
      typename enable_if<is_unsigned<T>::value>::type> {
    typedef T type; };

  template <> struct make_unsigned<char> { typedef unsigned char type; };
  template <> struct make_unsigned<signed char> { typedef unsigned char type; };
  template <> struct make_unsigned<signed int> { typedef unsigned int type; };
  template <> struct make_unsigned<signed long> {  // NOLINT(runtime/int)
      typedef unsigned long type;  // NOLINT(runtime/int)
  };
  template <> struct make_unsigned<signed long long> {  // NOLINT(runtime/int)
      typedef unsigned long long type;  // NOLINT(runtime/int)
  };

  template <class T, class Enable = void>
  struct make_signed;

  template <class T> struct make_signed<T,
      typename enable_if<is_signed<T>::value>::type> {
      typedef T type;
  };

  template <> struct make_signed<char> { typedef signed char type; };
  template <> struct make_signed<unsigned char> { typedef signed char type; };
  template <> struct make_signed<unsigned int> { typedef signed int type; };
  template <> struct make_signed<unsigned long> {  // NOLINT(runtime/int)
      typedef signed long type;  // NOLINT(runtime/int)
  };
  template <> struct make_signed<unsigned long long> {  // NOLINT(runtime/int)
      typedef signed long long type;  // NOLINT(runtime/int)
  };

  // TODO(nacitar): put this somewhere better?
  template <class T, class V> struct match_cv {
    typedef typename remove_cv<V>::type type;
  };
  template <class T, class V> struct match_cv<const volatile T, V> {
    typedef typename add_cv<V>::type type;
  };
  template <class T, class V> struct match_cv<volatile T, V> {
    typedef typename add_volatile<typename remove_const<V>::type>::type type;
  };
  template <class T, class V> struct match_cv<const T, V> {
    typedef typename add_const<typename remove_volatile<V>::type>::type type;
  };
}  // namespace detail
template <class T>
struct make_unsigned {
  typedef typename detail::match_cv<T,
      typename detail::make_unsigned<
          typename remove_cv<T>::type>::type>::type type;
};
template <class T>
struct make_signed {
  typedef typename detail::match_cv<T,
      typename detail::make_signed<
          typename remove_cv<T>::type>::type>::type type;
};
template< class T >
struct is_floating_point : integral_constant<bool,
    is_same<float, typename remove_cv<T>::type>::value  ||
    is_same<double, typename remove_cv<T>::type>::value  ||
    is_same<long double, typename remove_cv<T>::type>::value
                           > {};
// primary template
template<class>
struct is_function : false_type { };
// specialization for regular functions
template<class Ret, class... Args>
struct is_function<Ret(Args...)> : true_type {};
// specialization for variadic functions such as printf
template<class Ret, class... Args>
struct is_function<Ret(Args......)> : true_type {};
template< class T >

struct is_arithmetic : integral_constant<bool,
    is_integral<T>::value ||
    is_floating_point<T>::value> {
};
template< class T>
struct is_fundamental
  : integral_constant<
        bool,
        is_arithmetic<T>::value ||
        is_void<T>::value  ||
        is_same<nullptr_t, typename remove_cv<T>::type>::value
> {};

template< class T >
struct is_compound
    : integral_constant<bool, !is_fundamental<T>::value> {
};
template <class T> struct is_reference      : false_type {};
template <class T> struct is_reference<T&>  : true_type {};
template <class T> struct is_reference<T&&> : true_type {};



namespace detail {
  template< class T > struct is_pointer_helper     : false_type {};
  template< class T > struct is_pointer_helper<T*> : true_type {};
}  // namespace detail
template< class T > struct is_pointer
    : detail::is_pointer_helper<typename remove_cv<T>::type> {};

template<class T> struct is_lvalue_reference     : false_type {};
template<class T> struct is_lvalue_reference<T&> : true_type {};

template <class T> struct is_rvalue_reference      : false_type {};
template <class T> struct is_rvalue_reference<T&&> : true_type {};

template<class T> struct is_const          : false_type {};
template<class T> struct is_const<const T> : true_type {};

template<class T> struct is_volatile             : false_type {};
template<class T> struct is_volatile<volatile T> : true_type {};

template< class T >
struct alignment_of : integral_constant<size_t, alignof(T)> {};

template<class T>
struct rank : integral_constant<size_t, 0> {};
template<class T>
struct rank<T[]> : public integral_constant<size_t, rank<T>::value + 1> {};
template<class T, size_t N>
struct rank<T[N]> : public integral_constant<size_t, rank<T>::value + 1> {};

template<class T, unsigned N = 0>
struct extent : integral_constant<size_t, 0> {};
template<class T>
struct extent<T[], 0> : integral_constant<size_t, 0> {};
template<class T, unsigned N>
struct extent<T[], N> : integral_constant<size_t, extent<T, N-1>::value> {};
template<class T, size_t N>
struct extent<T[N], 0> : integral_constant<size_t, N> {};
template<class T, size_t I, unsigned N>
struct extent<T[I], N> : integral_constant<size_t, extent<T, N-1>::value> {};

template<class T>
struct remove_extent { typedef T type; };
template<class T>
struct remove_extent<T[]> { typedef T type; };
template<class T, size_t N>
struct remove_extent<T[N]> { typedef T type;};

template<class T>
struct remove_all_extents { typedef T type;};
template<class T>
struct remove_all_extents<T[]> {
    typedef typename remove_all_extents<T>::type type;
};
template<class T, size_t N>
struct remove_all_extents<T[N]> {
    typedef typename remove_all_extents<T>::type type;
};

template<size_t Len, size_t Align>
struct aligned_storage {
    struct type {
        alignas(Align) unsigned char data[Len];
    };
};
template< class T >
struct decay {
    typedef typename remove_reference<T>::type U;
    typedef typename conditional<
        is_array<U>::value,
        typename remove_extent<U>::type*,
        typename conditional<
            is_function<U>::value,
            typename add_pointer<U>::type,
            typename remove_cv<U>::type
        >::type
    >::type type;
};

template< class T >
using decay_t = typename decay<T>::type;

namespace type_traits_detail {

template <typename T, bool b>
struct add_rvalue_reference_helper {
  typedef T type;
};
template <typename T>
struct add_rvalue_reference_helper<T, true> {
  typedef T&& type;
};

}  // namespace type_traits_detail

template <typename T>
struct add_rvalue_reference {
  typedef typename type_traits_detail::add_rvalue_reference_helper<T,
      (!is_void<T>::value && !is_reference<T>::value)>::type type;
};

/**
 *  @brief  Convert a value to an rvalue.
 *  @param  __t  A thing of arbitrary type.
 *  @return The parameter cast to an rvalue-reference to allow moving it.
*/
template<typename _Tp>
constexpr typename remove_reference<_Tp>::type&&
    move(_Tp&& __t) noexcept {
  return static_cast<typename remove_reference<_Tp>::type&&>(__t);
}
/**
 *  @brief  Forward an rvalue.
 *  @return The parameter cast to the specified type.
 *
 *  This function is used to implement "perfect forwarding".
 */
template<typename _Tp>
constexpr _Tp&& forward(typename remove_reference<_Tp>::type&& __t) noexcept {
  static_assert(!is_lvalue_reference<_Tp>::value, "template argument"
      " substituting _Tp is an lvalue reference type");
  return static_cast<_Tp&&>(__t);
}

namespace detail {

template< class T >
struct is_member_pointer_helper         : std::false_type {};

template< class T, class U >
struct is_member_pointer_helper<T U::*> : std::true_type {};

template< class T >
struct is_member_function_pointer_helper : std::false_type {};

template< class T, class U>
struct is_member_function_pointer_helper<T U::*> : std::is_function<T> {};

}  // namespace detail

template< class T >
struct is_member_pointer :
    detail::is_member_pointer_helper<typename std::remove_cv<T>::type> {};

template< class T >
struct is_member_function_pointer : detail::is_member_function_pointer_helper<
    typename std::remove_cv<T>::type> {
};

template<class T>
struct is_member_object_pointer : std::integral_constant<bool,
    std::is_member_pointer<T>::value &&
    !std::is_member_function_pointer<T>::value> {
};

}  // namespace std

#endif  // INCLUDE_NX_CORE_EMBEDDED_TYPE_TRAITS_H_
