//
// Copyright (C) 2015 Jacob McIntosh <nacitar at ubercpp dot com>
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

/// @file types.h
/// @brief Simple typedefs that can be easily defined.

#ifndef INCLUDE_NX_CORE_TYPES_H_
#define INCLUDE_NX_CORE_TYPES_H_

#include "nx/core/os.h"

#ifndef NX_EMBEDDED
#include <type_traits>
#ifndef NX_TARGET_WINDOWS
#include <sys/types.h>  // pid_t
#endif
#include <cstddef>  // size_t, ptrdiff_t
#else
// Embedded has its own type_traits impl
#include "nx/core/embedded/type_traits.h"
// No cstddef either.
#include <stddef.h>  // size_t, ptrdiff_t
namespace std
{
  using ::ptrdiff_t;
  using ::size_t;
}
#endif


/// @brief Library namespace.
namespace nx {

#ifndef NX_EMBEDDED
#ifdef NX_TARGET_WINDOWS
/// @brief A type capable of holding a process identifier.
typedef DWORD pid_t;
#else
/// @brief A type capable of holding a process identifier.
typedef pid_t pid_t;
#endif
#endif

/// @brief A signed integer type able to represent the result of any valid
/// pointer subtraction operation.
typedef std::ptrdiff_t ptrdiff_t;

/// @brief An unsigned integer type that can store the maximum size of a
/// theoretically possible object of any type (including array).
typedef std::size_t size_t;

/// @brief A signed integer type the same size as size_t.
typedef std::make_signed<size_t>::type ssize_t;

}  // namespace nx

#endif  // INCLUDE_NX_CORE_TYPES_H_
