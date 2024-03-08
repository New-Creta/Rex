#include "rex_std/bonus/types.h"

#include "rex_std/iostream.h"
#include "rex_std/format.h"
#include "rex_fuzzy_test/rex_fuzzy_test.h"

namespace rex
{
  namespace fuzzy
  {
    class FuzzyTestingInfo
    {
    public:
      FuzzyTestingInfo()
        : m_num_iterations(0)
      {}

      card32 new_iteration()
      {
        ++m_num_iterations;
        return m_num_iterations;
      }

      card32 num_iterations() const
      {
        return m_num_iterations;
      }

    private:
      card32 m_num_iterations;
    } g_fuzzy_testing_info;
  }
}

// This is the entry point used by the compiler for the fuzzy testing
// for a list of commandline arguments enabled by the compiler
// visit: https://llvm.org/docs/LibFuzzer.html
extern "C" int LLVMFuzzerTestOneInput(const char* data, size_t size)
{
  rsl::cout << rsl::format("Iteration: {}", rex::fuzzy::g_fuzzy_testing_info.new_iteration()) << "\n";

  return rex::fuzzy::fuzzy_entry(rex::fuzzy::fuzz_span(data, static_cast<card32>(size)));
}
