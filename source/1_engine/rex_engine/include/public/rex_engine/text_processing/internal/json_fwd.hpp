//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT

#ifndef INCLUDE_NLOHMANN_JSON_FWD_HPP_
#define INCLUDE_NLOHMANN_JSON_FWD_HPP_

#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/map.h"
#include "rex_std/utility.h"
#include "rex_std/unordered_map.h"
#include "rex_std/array.h"
#include "rex_std/vector.h"
#include "rex_std/forward_list.h"
#include "rex_std/valarray.h"
#include "rex_std/ios.h"
#include "rex_std/iostream.h"
#include "rex_std/streambuf.h"
#include "rex_std/functional.h"

// #include <nlohmann/detail/abi_macros.hpp>
//     __ _____ _____ _____
//  __|  |   __|     |   | |  JSON for Modern C++
// |  |  |__   |  |  | | | |  version 3.11.3
// |_____|_____|_____|_|___|  https://github.com/nlohmann/json
//
// SPDX-FileCopyrightText: 2013-2023 Niels Lohmann <https://nlohmann.me>
// SPDX-License-Identifier: MIT



// This file contains all macro definitions affecting or depending on the ABI

#ifndef JSON_SKIP_LIBRARY_VERSION_CHECK
    #if defined(NLOHMANN_JSON_VERSION_MAJOR) && defined(NLOHMANN_JSON_VERSION_MINOR) && defined(NLOHMANN_JSON_VERSION_PATCH)
        #if NLOHMANN_JSON_VERSION_MAJOR != 3 || NLOHMANN_JSON_VERSION_MINOR != 11 || NLOHMANN_JSON_VERSION_PATCH != 3
            #warning "Already included a different version of the library!"
        #endif
    #endif
#endif

#define NLOHMANN_JSON_VERSION_MAJOR 3   // NOLINT(modernize-macro-to-enum)
#define NLOHMANN_JSON_VERSION_MINOR 11  // NOLINT(modernize-macro-to-enum)
#define NLOHMANN_JSON_VERSION_PATCH 3   // NOLINT(modernize-macro-to-enum)

#ifndef JSON_DIAGNOSTICS
    #define JSON_DIAGNOSTICS 0
#endif

#ifndef JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
    #define JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON 0
#endif

#if JSON_DIAGNOSTICS
    #define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS _diag
#else
    #define NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS
#endif

#if JSON_USE_LEGACY_DISCARDED_VALUE_COMPARISON
    #define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON _ldvcmp
#else
    #define NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_NO_VERSION
    #define NLOHMANN_JSON_NAMESPACE_NO_VERSION 0
#endif

// Construct the namespace ABI tags component
#define NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b) json_abi ## a ## b
#define NLOHMANN_JSON_ABI_TAGS_CONCAT(a, b) \
    NLOHMANN_JSON_ABI_TAGS_CONCAT_EX(a, b)

#define NLOHMANN_JSON_ABI_TAGS                                       \
    NLOHMANN_JSON_ABI_TAGS_CONCAT(                                   \
            NLOHMANN_JSON_ABI_TAG_DIAGNOSTICS,                       \
            NLOHMANN_JSON_ABI_TAG_LEGACY_DISCARDED_VALUE_COMPARISON)

// Construct the namespace version component
#define NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch) \
    _v ## major ## _ ## minor ## _ ## patch
#define NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(major, minor, patch) \
    NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT_EX(major, minor, patch)

#if NLOHMANN_JSON_NAMESPACE_NO_VERSION
#define NLOHMANN_JSON_NAMESPACE_VERSION
#else
#define NLOHMANN_JSON_NAMESPACE_VERSION                                 \
    NLOHMANN_JSON_NAMESPACE_VERSION_CONCAT(NLOHMANN_JSON_VERSION_MAJOR, \
                                           NLOHMANN_JSON_VERSION_MINOR, \
                                           NLOHMANN_JSON_VERSION_PATCH)
#endif

// Combine namespace components
#define NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b) a ## b
#define NLOHMANN_JSON_NAMESPACE_CONCAT(a, b) \
    NLOHMANN_JSON_NAMESPACE_CONCAT_EX(a, b)

#ifndef NLOHMANN_JSON_NAMESPACE
#define NLOHMANN_JSON_NAMESPACE               \
    nlohmann::NLOHMANN_JSON_NAMESPACE_CONCAT( \
            NLOHMANN_JSON_ABI_TAGS,           \
            NLOHMANN_JSON_NAMESPACE_VERSION)
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_BEGIN
#define NLOHMANN_JSON_NAMESPACE_BEGIN                \
    namespace nlohmann                               \
    {                                                \
    inline namespace NLOHMANN_JSON_NAMESPACE_CONCAT( \
                NLOHMANN_JSON_ABI_TAGS,              \
                NLOHMANN_JSON_NAMESPACE_VERSION)     \
    {
#endif

#ifndef NLOHMANN_JSON_NAMESPACE_END
#define NLOHMANN_JSON_NAMESPACE_END                                     \
    }  /* namespace (inline namespace) NOLINT(readability/namespace) */ \
    }  // namespace nlohmann
#endif


/*!
@brief namespace for Niels Lohmann
@see https://github.com/nlohmann
@since version 1.0.0
*/
NLOHMANN_JSON_NAMESPACE_BEGIN

/*!
@brief default JSONSerializer template argument

This serializer ignores the template arguments and uses ADL
([argument-dependent lookup](https://en.cppreference.com/w/cpp/language/adl))
for serialization.
*/
template<typename T = void, typename SFINAE = void>
struct adl_serializer;

/// a class to store JSON values
/// @sa https://json.nlohmann.me/api/basic_json/
template<template<typename U, typename V, typename... Args> class ObjectType =
         rsl::map,
         template<typename U, typename... Args> class ArrayType = rsl::vector,
         class StringType = rsl::string, class BooleanType = bool,
         class NumberIntegerType = rsl::int64,
         class NumberUnsignedType = rsl::uint64,
         class NumberFloatType = double,
         class AllocatorType = rsl::allocator,
         template<typename T, typename SFINAE = void> class JSONSerializer =
         adl_serializer,
         class BinaryType = rsl::vector<rsl::uint8>, // cppcheck-suppress syntaxError
         class CustomBaseClass = void>
class basic_json;

/// @brief JSON Pointer defines a string syntax for identifying a specific value within a JSON document
/// @sa https://json.nlohmann.me/api/json_pointer/
template<typename RefStringType>
class json_pointer;

/*!
@brief default specialization
@sa https://json.nlohmann.me/api/json/
*/
using json = basic_json<>;

/// @brief a minimal map-like container that preserves insertion order
/// @sa https://json.nlohmann.me/api/ordered_map/
template<class Key, class T, class IgnoredLess, class Allocator>
struct ordered_map;

/// @brief specialization that maintains the insertion order of object keys
/// @sa https://json.nlohmann.me/api/ordered_json/
using ordered_json = basic_json<nlohmann::ordered_map>;

NLOHMANN_JSON_NAMESPACE_END

#endif  // INCLUDE_NLOHMANN_JSON_FWD_HPP_
