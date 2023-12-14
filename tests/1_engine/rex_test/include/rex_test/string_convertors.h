#pragma once

#include "rex_test/catch2/catch.hpp"

#include "rex_std/string.h"
#include "rex_std/string_view.h"

namespace Catch {
  template<>
  struct StringMaker<rsl::string> {
    static std::string convert(rsl::string const& value)
    {
      std::string res("\"");
      res.append(value.data(), value.length());
      res += "\"";
      return res;
    }
  };

  template<>
  struct StringMaker<rsl::string_view> {
    static std::string convert(rsl::string_view const& value)
    {
      std::string res("\"");
      res.append(value.data(), value.length());
      res += "\"";
      return res;
    }
  };
}