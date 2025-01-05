#pragma once

#include <Windows.h>

namespace rex
{
  namespace win32
  {
    // these are workarounds C style anonymous structs with typedef
    struct WinUsnRecord : USN_RECORD
    {
    };

    struct WinUsnJournalData : USN_JOURNAL_DATA_V1
    {
    };

    struct WinReadUsnJournalData : READ_USN_JOURNAL_DATA_V0
    {
    };
  } // namespace win32
} // namespace rex