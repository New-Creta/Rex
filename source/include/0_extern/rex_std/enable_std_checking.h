// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: std_checking.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

// pragma once omitted on purpose

#ifndef ENABLE_STD_CHECKING
// #define ENABLE_STD_CHECKING
#endif

#ifdef ENABLE_STD_CHECKING
  #ifndef std
    #define std STATIC_WARNING("using std in codebase") std
  #endif
#endif