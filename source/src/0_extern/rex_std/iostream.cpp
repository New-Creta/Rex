// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: iostream.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

//-----------------------------------------------------------------------------
// https://en.cppreference.com/w/cpp/header/iostream
//
//
//-----------------------------------------------------------------------------

#include "rex_std/iostream.h"

#include "rex_std/bonus/iostream/console_buf.h"
#include "rex_std/internal/type_traits/is_same.h"

#include <Windows.h>

namespace rsl
{
  inline namespace v1
  {

    rsl::streambuf* cin_streambuf()
    {
      HANDLE input_handle = GetStdHandle(STD_INPUT_HANDLE);

      static console_buf<char8> buff(input_handle);
      return &buff;
    }
    rsl::streambuf* cout_streambuf()
    {
      HANDLE output_handle = GetStdHandle(STD_OUTPUT_HANDLE);

      static console_buf<char8> buff(output_handle);
      return &buff;
    }
    rsl::streambuf* cerr_streambuf()
    {
      HANDLE error_handle = GetStdHandle(STD_ERROR_HANDLE);

      static console_buf<char8> buff(error_handle);
      return &buff;
    }

    rsl::istream cin(cin_streambuf());   // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
    rsl::ostream cout(cout_streambuf()); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
    rsl::ostream cerr(cerr_streambuf()); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
    rsl::ostream clog(cerr_streambuf()); // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)

    namespace internal
    {
      class init_cout // NOLINT(cppcoreguidelines-special-member-functions)
      {
      public:
        init_cout()
        {
          cin.tie(&cout);
          cerr.tie(&cout);
        }
        ~init_cout()
        {
          clog.flush();
          cout.flush();
          cerr.flush();
        }
      };

      init_cout g_init_cout; // NOLINT(cppcoreguidelines-avoid-non-const-global-variables, fuchsia-statically-constructed-objects)
    }                        // namespace internal

  } // namespace v1
} // namespace rsl