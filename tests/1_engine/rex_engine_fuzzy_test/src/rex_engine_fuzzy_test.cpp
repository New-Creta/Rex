#include "rex_fuzzy_test/rex_fuzzy_test.h"

#include "rex_std/iostream.h"

namespace rex
{
  namespace fuzzy
  {
    int fuzzy_entry(const fuzz_span& input)
    {
      // Example code
      fuzz_span my_test_input = input;
      int test_val = make_fuzzy_object<int>(my_test_input);

      return 0;
    }
  }
}