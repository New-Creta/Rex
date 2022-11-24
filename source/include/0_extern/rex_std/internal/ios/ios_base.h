// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: ios_base.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/io/ios_base
//
// MSVC standard as well as llvm's implementation implement I/O functionality
// by using C standard 'FILE' structure. This is understandable as it really
// avoids reimplementing I/O functionality, it can instead build on top of
// C standard's. However, this adds a indirection and we inherit all the
// the functionality of C. both the good but also the bad.
// Part of the reason why Rex Standard Library was created was to avoid the
// dependency on C, therefore we implement this from scratch, without using
// C's FILE dependency.
//
// This implementation is for MSVC, targeting Windows.
// other platforms are currently not supported, when they do, expect this header
// to change to make it more generic.
//-----------------------------------------------------------------------------

#pragma once

#include "rex_std/internal/ios/io_types.h"
#include "rex_std/internal/type_traits/underlying_type.h"

namespace rsl
{
  inline namespace v1
  {

    namespace io
    {
      /// [09/Sep/2022] RSL Comment: to avoid having the following enum classes
      // be part of the ios_base class, we define them in an io namespace instead.
      enum class openmode : uint32
      {
        app,    // seek to the end of stream before each write
        binary, // open in binary mode
        in,     // open for reading
        out,    // open for writing
        trunc,  // discard the contents of the stream when opening
        ate     // seek to the end of stream immediately after open
      };

      enum class iostate : uint32
      {
        goodbit   = 0,        // no error
        badbit    = (1 << 0), // irrecoverable stream error
        failbit   = (1 << 1), // input/output operation failed
        eofbit    = (1 << 2), // associated input sequence has reached end-of-file
        noinitbit = (1 << 3)  // the stream is not yet initialized /// RSL Comment: Not in ISO C++ Standard at time of writing (09/Sep/2022)
      };

      namespace internal
      {
        using openmode_int = rsl::underlying_type_t<openmode>;
        using iostate_int  = rsl::underlying_type_t<iostate>;
      } // namespace internal

      constexpr openmode operator&(openmode lhs, openmode rhs)
      {
        const internal::openmode_int lhs_int = static_cast<internal::openmode_int>(lhs);
        const internal::openmode_int rhs_int = static_cast<internal::openmode_int>(rhs);

        return static_cast<openmode>(lhs_int & rhs_int);
      }
      constexpr openmode operator&=(openmode& lhs, openmode rhs)
      {
        lhs = lhs & rhs;
        return lhs;
      }
      constexpr openmode operator|(openmode lhs, openmode rhs)
      {
        const internal::openmode_int lhs_int = static_cast<internal::openmode_int>(lhs);
        const internal::openmode_int rhs_int = static_cast<internal::openmode_int>(rhs);

        return static_cast<openmode>(lhs_int | rhs_int);
      }
      constexpr openmode operator|=(openmode& lhs, openmode rhs)
      {
        lhs = lhs | rhs;
        return lhs;
      }

      constexpr iostate operator&(iostate lhs, iostate rhs)
      {
        internal::iostate_int const lhs_int = static_cast<internal::iostate_int>(lhs);
        internal::iostate_int const rhs_int = static_cast<internal::iostate_int>(rhs);

        return static_cast<iostate>(lhs_int & rhs_int);
      }
      constexpr iostate operator&=(iostate& lhs, iostate rhs)
      {
        lhs = lhs & rhs;
        return lhs;
      }
      constexpr iostate operator|(iostate lhs, iostate rhs)
      {
        internal::iostate_int const lhs_int = static_cast<internal::iostate_int>(lhs);
        internal::iostate_int const rhs_int = static_cast<internal::iostate_int>(rhs);

        return static_cast<iostate>(lhs_int | rhs_int);
      }
      constexpr iostate operator|=(iostate& lhs, iostate rhs)
      {
        lhs = lhs | rhs;
        return lhs;
      }

      enum class seekdir
      {
        beg, // the beginning of a stream
        end, // the ending of a stream
        cur, // the current position of stream position indicator
      };
    } // namespace io

    class ios_base
    {
    public:
      // you can't copy something that inherits from ios_base
      ios_base(const ios_base&) = delete;

      // destroys the ios_base object
      virtual ~ios_base() = default;

      // you can't assign to an ios_base object
      ios_base& operator=(const ios_base&) = delete;

      /// [09/Sep/2022] RSL Comment: Rex Standard Library ios_base provides no implemented member functions.
      // This is because Rex Standard Library doesn't provide locales, nor does it allow the user
      // to config global objects like rsl::cout with formatting rules.

      /// [09/Sep/2022] RSL Comment: Because Rex Standard Library wants to break the dependency on C.
      // it makes little sense of having sync_with_stdio as a member function.

    protected:
      // only derived classes can construct an ios_base class.
      // the derived class must call basic_ios::init() to complete initialization
      // before first use and before destructor
      ios_base() = default;
    };

  } // namespace v1
} // namespace rsl