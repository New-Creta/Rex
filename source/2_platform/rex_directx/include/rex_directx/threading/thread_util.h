#pragma once

#include "rex_std/thread.h"

#include <thread>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // For HRESULT
#include <comdef.h>  // For _com_error class (used to decode HR result codes).

namespace rex
{
  namespace renderer
  {
    namespace threading
    {
      // Set the name of an rsl::thread.
      // Useful for debugging.
      const DWORD MS_VC_EXCEPTION = 0x406D1388;

// Set the name of a running thread (for debugging)
#pragma pack(push, 8)
      typedef struct tagTHREADNAME_INFO
      {
        DWORD dwType;     // Must be 0x1000.
        LPCSTR szName;    // Pointer to name (in user addr space).
        DWORD dwThreadID; // Thread ID (-1=caller thread).
        DWORD dwFlags;    // Reserved for future use, must be zero.
      } THREADNAME_INFO;
#pragma pack(pop)

      void set_thread_name(rsl::thread& thread, const char* threadName);
    } // namespace threading
  }   // namespace renderer
} // namespace rex