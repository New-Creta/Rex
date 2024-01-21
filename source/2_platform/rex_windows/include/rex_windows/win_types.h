#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace win32
  {
    using HInstance = void*;
    using Hwnd      = void*;
    using Hdc       = void*;

    using WParam  = size_t;
    using LParam  = long long;
    using LResult = size_t;

#ifndef UNICODE
    using LPtStr = char*;
#else
    using LPtStr = wchar_t*;
#endif

    using DWord               = unsigned long;
    using WindowProcedureFunc = LResult(__stdcall*)(Hwnd, s32, WParam, LParam);
  } // namespace win32
} // namespace rex