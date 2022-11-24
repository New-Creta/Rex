// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: cout.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/internal/io/cout.h"

#include <Windows.h>

namespace rsl
{
    //HANDLE out_handle()
    //{
    //    static HANDLE out_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    //    return out_handle;
    //}

    //void CoutStream::flush()
    //{
    //    DWORD num_bytes_written = 0;
    //    WriteFile(out_handle(), stream_buff().stream_data(), stream_buff().stream_size(), &num_bytes_written, NULL);
    //    stream_buff().flush();
    //}
}