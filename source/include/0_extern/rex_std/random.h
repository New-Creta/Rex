// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: random.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/types.h"
#include "rex_std/disable_std_checking.h"
#include "rex_std/initializer_list.h"
#include "rex_std/std_alias_defines.h"

#include <random>

namespace rsl
{
  inline namespace v1
  {

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename UIntType, UIntType a, UIntType c, UIntType m>, linear_congruential_engine, UIntType, a, c, m);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename UIntType, size_t w, size_t n, size_t m, size_t r, UIntType a, size_t u, UIntType d, size_t s, UIntType b, size_t t, UIntType c, size_t l, UIntType f>, mersenne_twister_engine, UIntType, w, n, m, r,
                                  a, u, d, s, b, t, c, l, f);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename UIntType, size_t w, size_t s, size_t r>, subtract_with_carry_engine, UIntType, w, s, r);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Engine, size_t P, size_t R>, discard_block_engine, Engine, P, R);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Engine, size_t W, typename UIntType>, independent_bits_engine, Engine, W, UIntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename Engine, size_t K>, shuffle_order_engine, Engine, K);

    REX_STD_CLASS_ALIAS(minstd_rand0);
    REX_STD_CLASS_ALIAS(minstd_rand);
    REX_STD_CLASS_ALIAS(mt19937);
    REX_STD_CLASS_ALIAS(mt19937_64);
    REX_STD_CLASS_ALIAS(ranlux24_base);
    REX_STD_CLASS_ALIAS(ranlux48_base);
    REX_STD_CLASS_ALIAS(knuth_b);
    REX_STD_CLASS_ALIAS(default_random_engine);

    REX_STD_CLASS_ALIAS(random_device);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, uniform_int_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, uniform_real_distribution, RealType);

    REX_STD_CLASS_ALIAS(bernoulli_distribution);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, binomial_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, negative_binomial_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, geometric_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, poisson_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, exponential_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, gamma_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, weibull_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, extreme_value_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, normal_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, lognormal_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, chi_squared_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, cauchy_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, fisher_f_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, student_t_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename IntType = int32>, discrete_distribution, IntType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, piecewise_constant_distribution, RealType);

    REX_STD_TEMPLATED_CLASS_ALIAS(template <typename RealType = float64>, piecewise_linear_distribution, RealType);

    REX_STD_CLASS_ALIAS(seed_seq);

    REX_STD_FUNC_ALIAS(generate_canonical);

  } // namespace v1
} // namespace rsl

#include "rex_std/enable_std_checking.h"