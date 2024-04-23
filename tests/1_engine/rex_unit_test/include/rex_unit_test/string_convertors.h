#pragma once

#include "rex_unit_test/catch2/catch.hpp"

#include "rex_std/string.h"
#include "rex_std/string_view.h"

namespace Catch {
  template<>
  struct StringMaker<rsl::string> {
    static rsl::string convert(rsl::string const& value)
    {
      rsl::string res("\"");
      res.append(value.data(), value.length());
      res += "\"";
      return res;
    }
  };

  template<>
  struct StringMaker<rsl::string_view> {
    static rsl::string convert(rsl::string_view const& value)
    {
      rsl::string res("\"");
      res.append(value.data(), value.length());
      res += "\"";
      return res;
    }
  };
}