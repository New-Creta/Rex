// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: iostream.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/iostream.h"

#include <cstdio> // FILE, _get_stream_buffer_pointers

#include <iostream>

// ================
// Windows only!
// ================

//namespace rsl
//{
//    StreamBuff create_streambuf(FILE* file)
//    {
//        // Skip state checking, we'll implement it when we need it..
//        // just get the stream buffer pointers
//        char** base_ptr = nullptr;
//        char** next_ptr = nullptr;
//        int* length = nullptr;
//        ::_get_stream_buffer_pointers(file, &base_ptr, &next_ptr, &length);
//
//        return StreamBuff(*base_ptr, *next_ptr, *length);
//    }
//
//    CoutStream cout;
//    CoutStream clog;
//    CoutStream cerr;
//    CinStream cin;
//}