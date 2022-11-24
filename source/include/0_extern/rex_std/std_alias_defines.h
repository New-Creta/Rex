// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: std_alias_defines.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

// clang-format off
#ifdef REX_ENABLE_STD_ALIAS
#define REX_STD_FUNC_ALIAS(name)                  using std:: name 
#define REX_STD_CLASS_ALIAS(name)                 using name = std:: name 
#define REX_STD_TEMPLATED_CLASS_ALIAS(templ, name, ...)  templ using name = std:: name<__VA_ARGS__> 
#define REX_STD_OBJECT_ALIAS(name)                constexpr auto name = std:: name 
#define REX_STD_TEMPLATED_OBJECT_ALIAS(templ, name, ...) templ constexpr auto name = std:: name<__VA_ARGS__>
#else
#define REX_STD_FUNC_ALIAS(name)                  
#define REX_STD_CLASS_ALIAS(name)                 
#define REX_STD_TEMPLATED_CLASS_ALIAS(templ, name, ...)   
#define REX_STD_OBJECT_ALIAS(name)                
#define REX_STD_TEMPLATED_OBJECT_ALIAS(templ, name, ...) 

#endif
// clang-format on
