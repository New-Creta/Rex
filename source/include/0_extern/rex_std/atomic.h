// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: atomic.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/disable_std_checking.h"
#include "rex_std/std_alias_defines.h"

#include <atomic>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, atomic, T);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename T>, atomic, T);

    REX_STD_CLASS_ALIAS(atomic_flag);
    REX_STD_CLASS_ALIAS(memory_order);

    REX_STD_CLASS_ALIAS(atomic_bool);
    REX_STD_CLASS_ALIAS(atomic_char);
    REX_STD_CLASS_ALIAS(atomic_schar);
    REX_STD_CLASS_ALIAS(atomic_uchar);
    REX_STD_CLASS_ALIAS(atomic_short);
    REX_STD_CLASS_ALIAS(atomic_ushort);
    REX_STD_CLASS_ALIAS(atomic_int);
    REX_STD_CLASS_ALIAS(atomic_uint);
    REX_STD_CLASS_ALIAS(atomic_long);
    REX_STD_CLASS_ALIAS(atomic_ulong);
    REX_STD_CLASS_ALIAS(atomic_llong);
    REX_STD_CLASS_ALIAS(atomic_ullong);
    REX_STD_CLASS_ALIAS(atomic_char8_t);
    REX_STD_CLASS_ALIAS(atomic_char16_t);
    REX_STD_CLASS_ALIAS(atomic_char32_t);
    REX_STD_CLASS_ALIAS(atomic_wchar_t);
    REX_STD_CLASS_ALIAS(atomic_int8_t);
    REX_STD_CLASS_ALIAS(atomic_uint8_t);
    REX_STD_CLASS_ALIAS(atomic_int16_t);
    REX_STD_CLASS_ALIAS(atomic_uint16_t);
    REX_STD_CLASS_ALIAS(atomic_int32_t);
    REX_STD_CLASS_ALIAS(atomic_uint32_t);
    REX_STD_CLASS_ALIAS(atomic_int64_t);
    REX_STD_CLASS_ALIAS(atomic_uint64_t);
    REX_STD_CLASS_ALIAS(atomic_int_least8_t);
    REX_STD_CLASS_ALIAS(atomic_uint_least8_t);
    REX_STD_CLASS_ALIAS(atomic_int_least16_t);
    REX_STD_CLASS_ALIAS(atomic_uint_least16_t);
    REX_STD_CLASS_ALIAS(atomic_int_least32_t);
    REX_STD_CLASS_ALIAS(atomic_uint_least32_t);
    REX_STD_CLASS_ALIAS(atomic_int_least64_t);
    REX_STD_CLASS_ALIAS(atomic_uint_least64_t);
    REX_STD_CLASS_ALIAS(atomic_int_fast8_t);
    REX_STD_CLASS_ALIAS(atomic_uint_fast8_t);
    REX_STD_CLASS_ALIAS(atomic_int_fast16_t);
    REX_STD_CLASS_ALIAS(atomic_uint_fast16_t);
    REX_STD_CLASS_ALIAS(atomic_int_fast32_t);
    REX_STD_CLASS_ALIAS(atomic_uint_fast32_t);
    REX_STD_CLASS_ALIAS(atomic_int_fast64_t);
    REX_STD_CLASS_ALIAS(atomic_uint_fast64_t);
    REX_STD_CLASS_ALIAS(atomic_intptr_t);
    REX_STD_CLASS_ALIAS(atomic_uintptr_t);
    REX_STD_CLASS_ALIAS(atomic_size_t);
    REX_STD_CLASS_ALIAS(atomic_ptrdiff_t);
    REX_STD_CLASS_ALIAS(atomic_intmax_t);
    REX_STD_CLASS_ALIAS(atomic_uintmax_t);

#ifdef REX_ENABLE_WITH_CPP20
    REX_STD_CLASS_ALIAS(atomic_signed_lock_free);
    REX_STD_CLASS_ALIAS(atomic_unsigned_lock_free);
#endif // REX_ENABLE_WITH_CPP20

    REX_STD_FUNC_ALIAS(atomic_is_lock_free);
    REX_STD_FUNC_ALIAS(atomic_store);
    REX_STD_FUNC_ALIAS(atomic_store_explicit);
    REX_STD_FUNC_ALIAS(atomic_exchange);
    REX_STD_FUNC_ALIAS(atomic_exchange_explicit);
    REX_STD_FUNC_ALIAS(atomic_compare_exchange_weak);
    REX_STD_FUNC_ALIAS(atomic_compare_exchange_weak_explicit);
    REX_STD_FUNC_ALIAS(atomic_compare_exchange_strong);
    REX_STD_FUNC_ALIAS(atomic_compare_exchange_strong_explicit);
    REX_STD_FUNC_ALIAS(atomic_fetch_add);
    REX_STD_FUNC_ALIAS(atomic_fetch_add_explicit);
    REX_STD_FUNC_ALIAS(atomic_fetch_sub);
    REX_STD_FUNC_ALIAS(atomic_fetch_sub_explicit);
    REX_STD_FUNC_ALIAS(atomic_fetch_and);
    REX_STD_FUNC_ALIAS(atomic_fetch_and_explicit);
    REX_STD_FUNC_ALIAS(atomic_fetch_or);
    REX_STD_FUNC_ALIAS(atomic_fetch_or_explicit);
    REX_STD_FUNC_ALIAS(atomic_fetch_xor);
    REX_STD_FUNC_ALIAS(atomic_fetch_xor_explicit);
    REX_STD_FUNC_ALIAS(atomic_wait);
    REX_STD_FUNC_ALIAS(atomic_wait_explicit);
    REX_STD_FUNC_ALIAS(atomic_notify_one);
    REX_STD_FUNC_ALIAS(atomic_notify_all);
    REX_STD_FUNC_ALIAS(atomic_flag_test);
    REX_STD_FUNC_ALIAS(atomic_flag_test_explicit);
    REX_STD_FUNC_ALIAS(atomic_flag_test_and_set);
    REX_STD_FUNC_ALIAS(atomic_flag_test_and_set_explicit);
    REX_STD_FUNC_ALIAS(atomic_flag_clear);
    REX_STD_FUNC_ALIAS(atomic_flag_clear_explicit);
    REX_STD_FUNC_ALIAS(atomic_flag_wait);
    REX_STD_FUNC_ALIAS(atomic_flag_wait_explicit);
    REX_STD_FUNC_ALIAS(atomic_flag_notify_one);
    REX_STD_FUNC_ALIAS(atomic_flag_notify_all);
    REX_STD_FUNC_ALIAS(atomic_init);
    REX_STD_FUNC_ALIAS(kill_dependency);
    REX_STD_FUNC_ALIAS(atomic_thread_fence);
    REX_STD_FUNC_ALIAS(atomic_signal_fence);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"
