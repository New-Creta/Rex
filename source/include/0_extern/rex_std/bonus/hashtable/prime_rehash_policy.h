// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: prime_rehash_policy.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"

namespace rsl
{
  inline namespace v1
  {
    struct hash_required_result
    {
      count_t new_bucket_count;
      bool is_hash_required;
    };

    struct prime_rehash_policy
    {
    public:
      explicit prime_rehash_policy(float32 maxLoadFactor = 1.0f);

      float32 get_max_load_factor() const;

      static uint32 get_prev_bucket_count_only(uint32 bucketCountHint);
      uint32 get_prev_bucket_count(uint32 bucketCountHint) const;
      uint32 get_next_bucket_count(uint32 bucketCountHint) const;
      uint32 get_bucket_count(uint32 elementCount) const;

      hash_required_result is_rehash_required(uint32 bucketCount, uint32 elementCount, uint32 elementAdd) const;

    private:
      float32 m_max_load_factor;
      float32 m_growth_factor;
      mutable uint32 m_next_resize;
    };

  } // namespace v1
} // namespace rsl