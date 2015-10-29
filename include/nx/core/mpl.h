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
#include "nx/core/types.h"

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

/// @brief Accepts a type and optionally a value of that type as a parameter.
/// Can be used to make a boolean condition dependent upon an arbitrary
/// template parameter by adding to it: && Depend<Type>()
template <class T>
NX_FORCEINLINE constexpr bool Depend() {
  return true;
}
template <class T>
NX_FORCEINLINE constexpr bool Depend(const T&) {
  return true;
}

/// @brief A function pointer type returning the type Return and accepting the
/// provided argument types.
template <typename Return, typename... Arguments>
using Function = Return (*)(Arguments...);

/// @brief A member function pointer type for a member of Class which returns
/// the type Return and accepting the provided argument types.
template <typename Return, typename Class, typename... Arguments>
using MemberFunction = Return (Class::*)(Arguments...);

// No need to invoke integral constants... they resolve to themselves!

template <class T, T kValue>
class Constant : public std::integral_constant<T, kValue> {
};

/// @brief Meta-constant boolean
template <bool kValue>
class Bool : public Constant<bool, kValue> {
};

/// @brief Meta-constant int
template <int kValue>
class Int : public Constant<int, kValue> {
};

/// @brief Meta-constant unsigned int
template <unsigned int kValue>
class UInt : public Constant<unsigned int, kValue> {
};

/// @brief Meta-constant size_t
template <size_t kValue>
class Size : public Constant<size_t, kValue> {
};

/// @brief Meta-constant ssize_t
template <ssize_t kValue>
class SSize : public Constant<ssize_t, kValue> {
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
  static_assert(false && Depend<Fallback>(),
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

/// @brief Makes an integral type unsigned.
template <typename T>
using MakeUnsigned = Invoke<SetSigned<false, T>>;

/// @brief Makes an integral type signed.
template <typename T>
using MakeSigned = Invoke<SetSigned<true, T>>;

}  // namespace nx

#endif  // INCLUDE_NX_CORE_MPL_H_
