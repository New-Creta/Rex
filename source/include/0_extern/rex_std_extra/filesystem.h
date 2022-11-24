// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: filesystem.h
// Copyright � Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std_extra/filesystem/core_mounts.h"
#include "rex_std_extra/filesystem/open_flags.h"

#ifdef REX_PLATFORM_WINDOWS
#include "rex_std_extra/filesystem/win/win_file.h"
#include "rex_std_extra/filesystem/win/win_virtual_file_system.h"
#endif