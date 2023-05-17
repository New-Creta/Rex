#include "rex_engine/diagnostics/win/win_stacktrace.h"

// NOLINTBEGIN(llvm-include-order)
// clang-format off
#include <Windows.h>               // for CaptureStackBackTrace
#include <DbgHelp.h>               // for IMAGEHLP_LINE64, MAX_SYM_NAME, Sym...
#include <basetsd.h>               // for ULONG64, DWORD64
#include <winscard.h>              // for ULONG
#include <stddef.h>

#include "rex_std/bonus/string.h"  // for big_stack_string, operator""_big
// clang-format on
// NOLINTEND(llvm-include-order)

namespace rex
{
  namespace win
  {
    // this is a callback used when resolving symbols, useful for debugging
    BOOL CALLBACK symbol_callback(HANDLE /*hProcess*/, ULONG /*ActionCode*/, ULONG64 /*CallbackData*/, ULONG64 /*UserContext*/)
    {
      // Nothing to implement yet

      return 0;
    }

    // this initializes the symbols library
    bool load_symbols()
    {
      static bool initialised = false;
      if(!initialised)
      {
        SymSetOptions(SYMOPT_DEFERRED_LOADS | SYMOPT_EXACT_SYMBOLS | SYMOPT_LOAD_LINES | SYMOPT_DEBUG | SYMOPT_UNDNAME);
        if(SymInitialize(GetCurrentProcess(), nullptr, TRUE) == 0)
        {
          SymCleanup(GetCurrentProcess());
        }

        SymRegisterCallback64(GetCurrentProcess(), symbol_callback, NULL);
        initialised = true;
      }

      return initialised;
    }

    // can't inline this, as we're doing a stacktrace, we want to be consistent.
    __declspec(noinline) CallStack load_stack_pointers(card32 framesToSkip)
    {
      CallStack stacks_pointers;
      const card32 frames_to_skip = framesToSkip;

      CaptureStackBackTrace(frames_to_skip, static_cast<DWORD>(stacks_pointers.size()), stacks_pointers.data(), nullptr);

      return stacks_pointers;
    }

    rsl::big_stack_string get_undecorated_name(const char* symbolName)
    {
      rsl::array<char, MAX_SYM_NAME> undecorated_name;
      UnDecorateSymbolName(symbolName, undecorated_name.data(), undecorated_name.size(), UNDNAME_COMPLETE);
      return rsl::big_stack_string(undecorated_name.data());
    }

    rsl::stacktrace_entry get_stack_pointer_line_info(HANDLE process, DWORD64 addr, DWORD64* displacement, const char* symbolName)
    {
      const rsl::big_stack_string undecorated_name = get_undecorated_name(symbolName);

      IMAGEHLP_LINE64 line {};
      line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
      if(SymGetLineFromAddr64(process, addr, reinterpret_cast<DWORD*>(displacement), &line) == TRUE) // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
      {
        return rsl::stacktrace_entry(reinterpret_cast<void*>(addr), rsl::big_stack_string(line.FileName), undecorated_name, static_cast<card32>(line.LineNumber)); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
      }
      else
      {
        // NOLINTBEGIN(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
        return rsl::stacktrace_entry(reinterpret_cast<void*>(addr), "Unable to retrieve file name"_big, "unable to retrieve func name"_big, -1);
        // NOLINTEND(cppcoreguidelines-pro-type-reinterpret-cast, performance-no-int-to-ptr)
      }
    }

    // we split up a resolved callstack from a regular callstack to save memory
    // we only resolve a callstack when we need to as it's a performant heavy process
    // and requires more memory to do so.
    ResolvedCallstack::ResolvedCallstack(const CallStack& callstack)
        : m_resolved_stacktrace()
        , m_size(0)
    {
      // make sure the symbols library is initialized, but do this only once
      [[maybe_unused]] static const bool s_initialised = load_symbols();

      // some boilerplate, preparing variables for stack tracing
      HANDLE process             = GetCurrentProcess();
      DWORD64 displacement       = 0;
      constexpr card32 buff_size = (sizeof(SYMBOL_INFO) + MAX_SYM_NAME * sizeof(TCHAR) + sizeof(ULONG64) - 1) / sizeof(ULONG64);
      rsl::array<ULONG64, buff_size> buffer;
      PSYMBOL_INFO symbol_info = reinterpret_cast<PSYMBOL_INFO>(buffer.data()); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)

      symbol_info->SizeOfStruct = sizeof(SYMBOL_INFO);
      symbol_info->MaxNameLen   = MAX_SYM_NAME;

      const card32 max_count = (rsl::min)(m_resolved_stacktrace.size(), callstack.size());
      for(card32 i = 0; i < max_count && callstack[i] != nullptr; ++i)
      {
        // the callstack passed in to the ctor only holds the pointers
        // we want the human readable information about these addresses
        void* stack_ptr   = callstack[i];
        const uint64 addr = *reinterpret_cast<uint64*>(&stack_ptr); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        rsl::stacktrace_entry stack_entry;

        // API call to convert an address into something human readable
        if(SymFromAddr(process, addr, &displacement, symbol_info) != 0)
        {
          stack_entry = get_stack_pointer_line_info(process, addr, &displacement, symbol_info->Name);

          // hack to check for an address we couldn't resolve
          if(addr != 0xcccccccc)
          {
            m_resolved_stacktrace[i] = stack_entry;
            ++m_size;
          }
        }
      }
    }

    const rsl::array<rsl::stacktrace_entry, g_max_stack_entries>& ResolvedCallstack::pointers() const
    {
      return m_resolved_stacktrace;
    }

    count_t ResolvedCallstack::size() const
    {
      return m_size;
    }

    const rsl::stacktrace_entry& ResolvedCallstack::operator[](count_t idx) const
    {
      return m_resolved_stacktrace[idx];
    }

    CallStack current_callstack()
    {
      return load_stack_pointers(0);
    }
  } // namespace win
} // namespace rex