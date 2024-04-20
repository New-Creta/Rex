#include "rex_unit_test/test_string.h"

#include <random>

namespace rex::test
{
  void create_random_string(rsl::string& str, s32 length)
  {
    rsl::small_stack_string chars("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890");
    for (card32 i = 0; i < length; ++i)
    {
      const card32 random_idx = std::rand() % chars.length(); // NOLINT(cert-msc50-cpp, concurrency-mt-unsafe)
      str += chars[random_idx];
    }
  }
}