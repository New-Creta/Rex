#include "rex_fuzzy_test/rex_fuzzy_test.h"

#include "rex_std/iostream.h"

namespace rex
{
  namespace fuzzy
  {
    int fuzzy_entry(fuzz_span input)
    {

      int test_val = rex::fuzzy::make_fuzzy_object<int>(input);

      return 0;
    }
  }
}