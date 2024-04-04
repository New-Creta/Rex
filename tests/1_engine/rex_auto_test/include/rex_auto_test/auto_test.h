#pragma once

#include "rex_std/string_view.h"

namespace rex
{
  struct PlatformCreationParams;
  struct ApplicationCreationParams;

  namespace auto_test
  {
    class AutoTest
    {
    public:
      using auto_test_entry = rex::ApplicationCreationParams(*)(rex::PlatformCreationParams&);

      AutoTest(rsl::string_view cmdline, auto_test_entry entryFunc);

      bool is_enabled(rsl::string_view cmdline) const;

      ApplicationCreationParams launch(rex::PlatformCreationParams& platformParams);

    private:
      rsl::string_view m_cmd_line;
      auto_test_entry m_entry_func;
    };

    rex::ApplicationCreationParams invalid_app_params(rex::PlatformCreationParams&& platformParams);

    // define this function in your auto test project.
    // it should return all possible auto tests it can run
    extern rsl::vector<AutoTest> auto_test_entry();
  }
}