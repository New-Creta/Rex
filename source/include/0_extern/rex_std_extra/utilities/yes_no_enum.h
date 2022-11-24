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

#pragma once

namespace rsl
{
  class YesNoEnum
  {
  public:
    enum EYesNoEnum
    {
      No = 0,
      Yes = 1
    };

    YesNoEnum(EYesNoEnum value);

    operator bool() const;

  private:
    EYesNoEnum m_value;
  };
}

#define DEFINE_YES_NO_ENUM(ClassName) \
class ClassName : public rsl::YesNoEnum \
{\
public:\
ClassName(EYesNoEnum value)\
: rsl::YesNoEnum(value)\
{}\
\
explicit ClassName(bool value)\
: rsl::YesNoEnum(value ? EYesNoEnum::Yes : EYesNoEnum::No)\
{}\
};