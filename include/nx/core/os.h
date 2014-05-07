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

/// @file os.h
/// @brief Platform detection and initialization; required everywhere.

#ifndef INCLUDE_NX_CORE_OS_H_
#define INCLUDE_NX_CORE_OS_H_

#include <limits.h>  // CHAR_BIT and PATH_MAX (on linux)

// Toolchain detection
#if defined(__clang__)
  /// @brief Set if the toolchain in use is Clang
  #define NX_TC_CLANG 1
#elif defined(__GNUC__)
  #ifndef __has_attribute
    #define __has_attribute(x) 0
  #endif
  #ifndef __has_builtin
    #define __has_builtin(x) 0
  #endif
  #ifndef __has_extension
    #define __has_extension(x) 0
  #endif
  /// @brief Set to GCC's version number as an integer if using GCC
  #define NX_TC_GCC (__GNUC__ * 10000 \
      + __GNUC_MINOR__ * 100 \
      + __GNUC_PATCHLEVEL__)
  #if (NX_TC_GCC == 0)
    #warning "Could not get GCC version number accurately"
    #define NX_TC_GCC 1
  #endif
#elif defined(_MSC_VER)
  /// @brief Set if the toolchain in use is visual studio
  #define NX_TC_VS 1
#endif

// C++11 requirement
#if (__cplusplus < 201103L) || (defined(NX_TC_GCC) && NX_TC_GCC < 40801)
  #error "This library is written with c++11 in mind; backward" \
      " compatibility has been removed.  If using gcc, this requires 4.8.1+"
#endif

// Target detection
#if \
    defined(WINDOWS) || \
    defined(WIN32) || defined(_WIN32) || \
    defined(WIN64) || defined(_WIN64)
  /// @brief Defined if build target is Windows
  #define NX_TARGET_WINDOWS 1
#elif \
    defined(__linux) || defined(__linux__) || defined(linux) || \
    defined(__gnu_linux__) || defined(LINUX)

  /// @brief Defined if build target is Linux
  #define NX_TARGET_LINUX 1
#elif defined(__APPLE__)
  /// @brief Defined if build target is Mac
  #define NX_TARGET_MAC 1
#elif defined(__SVR4) && defined(__sun)
  /// @brief Defined if build target is Solaris
  #define NX_TARGET_SOLARIS 1
#elif defined(__AVR__)
  /// @brief Defined if build target is AVR
  #define NX_TARGET_AVR 1
  /// @brief Defined if build target is an embedded platform
  #define NX_EMBEDDED 1
#else
  /// @brief Defined if build target is an unknown platform
  #define NX_TARGET_OTHER 1
#endif

// Compiler features
#if defined(NX_TC_GCC) || defined(NX_TC_CLANG)
  /// @brief Makes a best-effort to force the compiler to inline a function.
  #if defined(NX_TC_GCC) || __has_attribute(always_inline)
    #define NX_FORCEINLINE inline __attribute__((always_inline))
  #else
    #define NX_FORCEINLINE inline
  #endif

  #if defined(NX_TC_GCC) || __has_builtin(__builtin_expect)
    /// @brief Pass the conditional statement of an if statement to inform the
    /// compiler to structure branches expecting that the value is true.
    #define NX_LIKELY(x) __builtin_expect((x), 1)
    /// @brief Pass the conditional statement of an if statement to inform the
    /// compiler to structure branches expecting that the value is false.
    #define NX_UNLIKELY(x) __builtin_expect((x), 0)
  #else
    /// @brief Pass the conditional statement of an if statement to inform the
    /// compiler to structure branches expecting that the value is true.
    #define NX_LIKELY(x) (x)
    /// @brief Pass the conditional statement of an if statement to inform the
    /// compiler to structure branches expecting that the value is false.
    #define NX_UNLIKELY(x) (x)
  #endif
  #if defined(NX_TC_GCC) || __has_extension(attribute_deprecated_with_message)
    /// @brief Marks a function or variable as deprecated.
    #define NX_DEPRECATED(decl, msg) decl __attribute__((deprecated(msg)))
  #else
    /// @brief Marks a function or variable as deprecated.  However, this macro
    /// is NOT IMPLEMENTED on this platform.
    #define NX_DEPRECATED(decl, msg) decl
  #endif
#else
  /// @brief Makes a best-effort to force the compiler to inline a function.
  #define NX_FORCEINLINE inline
  /// @brief Pass the conditional statement of an if statement to inform the
  /// compiler to structure branches expecting that the value is true.
  #define NX_LIKELY(x) (x)
  /// @brief Pass the conditional statement of an if statement to inform the
  /// compiler to structure branches expecting that the value is false.
  #define NX_UNLIKELY(x) (x)

  #if defined(NX_TC_VS)
    /// @brief Marks a function or variable as deprecated.
    #define NX_DEPRECATED(decl, msg) __declspec(deprecated(msg)) decl
  #else
    /// @brief Marks a function or variable as deprecated.  However, this macro
    /// is NOT IMPLEMENTED on this platform.
    #define NX_DEPRECATED(decl, msg) decl
  #endif
#endif

// OS initialization/ensuring important system defines are set
#if defined(NX_TARGET_WINDOWS)
  // Undefine existing values first to silence warnings
  #undef WINVER
  #undef _WIN32_WINNT

  /// @brief Informs the windows headers we are targetting windows 7
  #define WINVER 0x0601
  /// @brief Informs the windows headers we are targetting windows 7
  #define _WIN32_WINNT 0x0601
  #ifndef NOMINMAX
    /// @brief Prevents the windows headers from defining min/max as macros.
    #define NOMINMAX
  #endif
  #include <windows.h>
  #ifndef PATH_MAX
    /// @brief Best guess per platform includes as to the maximum length of a
    /// path.
    #define PATH_MAX MAX_PATH
  #endif
#else
  /// @brief Best guess per platform includes as to the maximum length of a
  /// path.
  #define MAX_PATH PATH_MAX
#endif

#ifndef CHAR_BIT
  #error "header <limits.h> does not define CHAR_BIT, either explicitly " \
      "define it here or include the appropriate header!"
#endif

#endif  // INCLUDE_NX_CORE_OS_H_
