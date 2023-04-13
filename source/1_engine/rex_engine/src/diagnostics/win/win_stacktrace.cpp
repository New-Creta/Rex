#include "rex_engine/diagnostics/win/win_stacktrace.h"

// NOLINTBEGIN(llvm-include-order)
// clang-format off
#include <Windows.h> // this needs to be included before DbgHelp.h
#include <DbgHelp.h>
// clang-format on
// NOLINTEND(llvm-include-order)

namespace rex
{
  namespace win
  {
    BOOL CALLBACK symbol_callback(HANDLE /*hProcess*/, ULONG /*ActionCode*/, ULONG64 /*CallbackData*/, ULONG64 /*UserContext*/)
    {
      // Nothing to implement yet

      return 0;
    }

    bool load_symbols()
    {
      static bool initialised = false;
      if(!initialised)
      {
        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_EXACT_SYMBOLS | SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_UNDNAME);
        if(SymInitialize(GetCurrentProcess(), NULL, TRUE) == 0)
        {
          SymCleanup(GetCurrentProcess());
        }

        SymRegisterCallback64(GetCurrentProcess(), symbol_callback, NULL);
        initialised = true;
      }

      return initialised;
    }

    __declspec(noinline) CallStack load_stack_pointers(card32 framesToSkip)
    {
      CallStack stacks_pointers;
      const card32 frames_to_skip = framesToSkip + 3;

      CaptureStackBackTrace(frames_to_skip, static_cast<DWORD>(stacks_pointers.size()), stacks_pointers.data(), NULL);

      return stacks_pointers;
    }

    CallStack current_callstack()
    {
      return load_stack_pointers(0);
    }
  } // namespace win
} // namespace rex