#pragma once

#include "rex_std/string_view.h"
#include "rex_std/memory.h"
#include "rex_std/algorithm.h"

namespace rex
{
  namespace fuzzy
  {
    using fuzz_span = rsl::basic_string_view<char>;

    // Creates an object by copying N bytes into a trivial object
    // The input is reduced by the number of bytes copied
    template <typename T>
    T make_fuzzy_object(fuzz_span& input)
    {
      static_assert(rsl::is_trivial_v<T>, "Fuzzy object type needs to be trivial");

      T result{};

      card32 bytes_used = rsl::min(input.size(), static_cast<card32>(sizeof(result)));

      rsl::memcpy(&result, input.data(), bytes_used);

      input.remove_prefix(bytes_used);

      return result;
    }

    // This is the entry point for projects using fuzzy testing
    // This acts the same way as app_entry in Rex engine
    // 
    // This is made const reference so users have to copy it
    // They can create objects from the copied version while
    // the original one is left untouched
    // making the it easier to write multiple test
    // each creating their own objects starting from the beginning
    // of the input and the first test doesn't affect the input
    // to be passed in to the second
    extern int fuzzy_entry(const fuzz_span& input);
  }
}