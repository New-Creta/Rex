// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: cstring.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {

#define NULL 0 // NOLINT

#if defined(REX_PLATFORM_X86)
    using size_t = uint32;
#elif defined(REX_PLATFORM_X64)
    using size_t = uint64;
#else
  #error "No platform specified"
#endif

    // copies one string to another
    char8* strcpy(char8* dest, const char8* src);
    // copies a at most "count" characters to destination.
    // if the null termination char is reached, additional null characters are written.
    char8* strncpy(char8* dest, const char8* src, count_t count);
    // appends a copy of src to the end of dest
    char8* strcat(char8* dest, const char8* src);
    // at most count characters are appended from src to dest
    // it's possible this function copies count + 1
    // if the null termination char needs to be copied over after count characters have been copied
    char8* strncat(char8* dest, const char8* src, count_t count);
    // transforms src into a string such that comparing 2 transformed strings
    // with strcmp gives the same result as you'd compare the original strings with strcoll
    count_t strxfrm(char8* dest, const char8* src, count_t count);
    // returns the lengths of a string
    count_t strlen(const char8* str);
    // compares 2 strings lexicographically
    int32 strcmp(const char8* lhs, const char8* rhs);
    // compares at most count characters lexicographically
    int32 strncmp(const char8* lhs, const char8* rhs, count_t count);
    int32 strcoll(const char8* lhs, const char8* rhs);
    // finds the first occurrence of a character
    const char8* strchr(const char8* str, char8 ch);
    // finds the first occurrence of a character
    char8* strchr(char8* str, char8 ch);
    // finds the last occurrence of a character
    const char8* strrchr(const char8* str, char8 ch);
    // finds the last occurrence of a character
    char8* strrchr(char8* str, char8 ch);
    // calculates the length of the initial segment in "target"
    // that only contains characters from "toFind"
    count_t strspn(const char8* target, const char8* toFind);
    // calculates the length of the initial segment in "target"
    // that only contains characters not from "toFind"
    count_t strcspn(const char8* target, const char8* notToFind);
    // finds the first location of any character from a set of separators
    const char8* strpbrk(const char8* target, const char8* toFind);
    // finds the first location of any character from a set of separators
    char8* strpbrk(char8* target, const char8* toFind);
    // finds the first occurrence of substring of characters
    const char8* strstr(const char8* str, const char8* substr);
    // finds the first occurrence of substring of characters
    char8* strstr(char8* str, const char8* substr);
    char8* strtok(char8* str, const char8* delim);

    /// RSL Comment: Different from ISO C++ Standard at time of writing (09/Aug/2022)
    // Rex doesn't define character array manipulation functions in cstring, but defines them in memory instead.
    // these functions are:
    // memchr
    // memcmp
    // memset
    // memcpy
    // memmove

  } // namespace v1
} // namespace rsl

// #include "rex_std/disable_std_checking.h"
// #include "rex_std/std_alias_defines.h"
//
// #include <cstring>
//
// namespace rsl { inline namespace v1 {
//
//// REX_STD_FUNC_ALIAS(strxfrm);
//// REX_STD_FUNC_ALIAS(strcoll);
//// REX_STD_FUNC_ALIAS(strtok);
//// REX_STD_FUNC_ALIAS(strerror);
//
// }}
//
// #include "rex_std/enable_std_checking.h"