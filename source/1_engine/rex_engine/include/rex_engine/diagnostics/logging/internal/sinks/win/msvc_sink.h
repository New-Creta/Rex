
#pragma once

#if defined(_WIN32)

  #include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
  #include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

// Avoid including windows.h (https://stackoverflow.com/a/30741042)
extern "C" __declspec(dllimport) void __stdcall OutputDebugStringA(const char* lpOutputString);
extern "C" __declspec(dllimport) int __stdcall IsDebuggerPresent();

namespace rexlog
{
  namespace sinks
  {
    /*
     * MSVC sink (logging using OutputDebugStringA)
     */
    template <typename Mutex>
    class MSVCSink : public BaseSink<Mutex>
    {
    public:
      MSVCSink() = default;
      MSVCSink(bool check_debugger_present)
          : check_debugger_present_ {check_debugger_present} {};

    protected:
      void sink_it_impl(const details::LogMsg& msg) override
      {
        if(check_debugger_present_ && !IsDebuggerPresent())
        {
          return;
        }
        memory_buf_t formatted;
        BaseSink<Mutex>::m_formatter->format(msg, formatted);
        formatted.push_back('\0'); // add a null terminator for OutputDebugString
        OutputDebugStringA(formatted.data());
      }

      void flush_it_impl() override {}

      bool check_debugger_present_ = true;
    };

    using MSVCSinkMt = MSVCSink<rsl::mutex>;
    using MSVCSinkSt = MSVCSink<details::NullMutex>;

    using WinDebugSinkMt = MSVCSinkMt;
    using WinDebugSinkSt = MSVCSinkSt;

  } // namespace sinks
} // namespace rexlog

#endif
