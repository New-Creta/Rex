// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: yes_no_enum.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================



#include "rex_std_extra/utilities/yes_no_enum.h"

rsl::YesNoEnum::YesNoEnum(EYesNoEnum value)
  : m_value(value)
{}

rsl::YesNoEnum::operator bool() const
{
  return m_value == EYesNoEnum::Yes;
}