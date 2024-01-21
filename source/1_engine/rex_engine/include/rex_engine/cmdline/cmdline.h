#pragma once

#include "rex_std/bonus/string.h"

#include "rex_engine/string/stringid.h"

namespace rex
{
  namespace cmdline
  {


    struct Argument
    {
    public:
      Argument(rsl::wstring_view name, rsl::wstring_view desc, rsl::wstring_view module);

      StringID name_id;
      rsl::wstring_view name;
      rsl::wstring_view desc;
      rsl::wstring_view module; // the module (eg. RexEngine) this argument is defined for, very useful for debugging as well as for the help command
    };

    void init(rsl::wstring_view cmdLine);
    void shutdown();
    void print_args();

    // This is used to scan if a certain argument is specified
    rsl::optional<rsl::wstring_view> get_argument(rsl::wstring_view arg);

    // This is used to print the entire commandline and should not be used for parsing
    rsl::wstring_view get();
  } // namespace cmdline
} // namespace rex