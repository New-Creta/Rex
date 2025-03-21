#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
  namespace win
  {
    using HInstance = void*;
    using Hwnd = void*;
    using Hdc = void*;

    using WParam = size_t;
    using LParam = long long;
    using LResult = size_t;

#ifndef UNICODE
    using LPtStr = char*;
#else
    using LPtStr = wchar_t*;
#endif

    using HResult = long; // HRESULT
    using DWord = unsigned long;
    using WindowProcedureFunc = LResult(__stdcall*)(Hwnd, s32, WParam, LParam);
  } // namespace win
} // namespace rex