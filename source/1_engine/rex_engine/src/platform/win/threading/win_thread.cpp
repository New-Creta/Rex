#include "rex_engine/platform/win/threading/win_thread.h"

#include "rex_engine/platform/win/crash_reporter/win_crash_handler.h"

#include "rex_std/functional.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

namespace rex
{
  namespace threading
  {
    namespace internal
    {
      rsl::function<void()> crash_guard_thread_entry(rsl::function<void()>&& callable)
      {
        auto wrapped_callable = [func = rsl::move(callable)]()
        {
          __try
          {
            func();
          }
          __except (rex::win::report_crash_from_thread(GetExceptionInformation()), EXCEPTION_CONTINUE_SEARCH)
          {
            // Do nothing here as the code here doesn't get executed due to EXCEPTION_CONTINUE_SEARCH
            // handle crashing in the report_crash() function, then exit
            (void)0;
          }
        };
        return wrapped_callable;
      }
    }
  }
}