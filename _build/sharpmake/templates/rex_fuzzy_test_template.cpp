#include "rex_fuzzy_test/rex_fuzzy_test.h"

namespace rex
{
  namespace fuzzy
  {
    // The entry point of the fuzzy test
    // "input" is a buffer with random size and content, different every time it's called.
    int fuzzy_entry(fuzz_span input)
    {
      // Use the input to create a value with random content.
      int test_val = rex::fuzzy::make_fuzzy_object<int>(input);

      // Return 0 if the test succeeds
      return 0;
    }
  }
}