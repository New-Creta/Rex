//--------------------------------------------------
// File:    win_message_box.h
// Created: 1/21/2023 1:04:17 AM
//
// Created By: nickdb
// 
// Copyright (c) REX
//--------------------------------------------------

#pragma once

#include "rex_std/string_view.h"

namespace rex::win
{
  int32 message_box(const rsl::string_view msg);
  int32 message_box(const rsl::string_view msg, const rsl::string_view title);

  // Returns true if yes is pressed
  bool yes_no_message_box(const rsl::string_view msg);
  bool yes_no_message_box(const rsl::string_view msg, const rsl::string_view title);
}