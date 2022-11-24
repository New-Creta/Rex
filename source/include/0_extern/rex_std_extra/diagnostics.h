// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
// 
// File: diagnostics.h
// Copyright � Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std_extra/diagnostics/assert.h"
#include "rex_std_extra/diagnostics/console_colors.h"
#include "rex_std_extra/diagnostics/logging.h"
#include "rex_std_extra/diagnostics/log_message.h"

#ifdef REX_PLATFORM_WINDOWS
#include "rex_std_extra/diagnostics/win/hr_call.h"
#include "rex_std_extra/diagnostics/win/win_call.h"
#include "rex_std_extra/diagnostics/win/win_debug.h"
#include "rex_std_extra/diagnostics/win/win_error.h"
#include "rex_std_extra/diagnostics/win/win_message_box.h"
#include "rex_std_extra/diagnostics/win/win_stack_tracing.h"
#include "rex_std_extra/diagnostics/win/win_console.h"
#endif