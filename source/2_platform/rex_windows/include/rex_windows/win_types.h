//--------------------------------------------------
// File:    win_types.h
// Created: 3/24/2022 3:44:15 PM
//
// Created By: nickdb
// 
// Copyright (c) REX
//--------------------------------------------------

#pragma once

#include "rex_engine/types.h"

namespace rex
{
    namespace win32
    {
        using HInstance = void*;
        using Hwnd = void*;
        using Hdc = void*;

        using WParam = size_t;
        using LParam = size_t;
        using LResult = size_t;

#ifndef UNICODE
        using LPtStr = char*;
#else 
        using LPtStr = wchar_t*
#endif

        using DWord = unsigned long;
        using WindowProcedureFunc = LResult(__stdcall*)(Hwnd, s32, WParam, LParam);
    }
}