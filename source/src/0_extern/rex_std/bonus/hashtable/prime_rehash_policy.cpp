// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: prime_rehash_policy.cpp
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#include "rex_std/bonus/hashtable/prime_rehash_policy.h"

#include "rex_std/internal/algorithm/lower_bound.h"
#include "rex_std/internal/algorithm/max.h"
#include "rex_std/internal/algorithm/upper_bound.h"
#include "rex_std/internal/math/ceil.h"
#include "rex_std/internal/utility/size.h"

namespace rsl
{
  inline namespace v1
  {

    /// g_prime_number_array
    ///
    /// This is an array of prime numbers. This is the same set of prime
    /// numbers suggested by the C++ standard proposal. These are numbers
    /// which are separated by 8% per entry.
    ///
    /// To consider: Allow the user to specify their own prime number array.
    ///

    // NOLINTBEGIN
    const uint32 g_prime_number_array[] = {
        2u,          3u,          5u,          7u,          11u,         13u,         17u,         19u,         23u,         29u,         31u,         37u,         41u,         43u,         47u,         53u,         59u,         61u,
        67u,         71u,         73u,         79u,         83u,         89u,         97u,         103u,        109u,        113u,        127u,        137u,        139u,        149u,        157u,        167u,        179u,        193u,
        199u,        211u,        227u,        241u,        257u,        277u,        293u,        313u,        337u,        359u,        383u,        409u,        439u,        467u,        503u,        541u,        577u,        619u,
        661u,        709u,        761u,        823u,        887u,        953u,        1031u,       1109u,       1193u,       1289u,       1381u,       1493u,       1613u,       1741u,       1879u,       2029u,       2179u,       2357u,
        2549u,       2753u,       2971u,       3209u,       3469u,       3739u,       4027u,       4349u,       4703u,       5087u,       5503u,       5953u,       6427u,       6949u,       7517u,       8123u,       8783u,       9497u,
        10273u,      11113u,      12011u,      12983u,      14033u,      15173u,      16411u,      17749u,      19183u,      20753u,      22447u,      24281u,      26267u,      28411u,      30727u,      33223u,      35933u,      38873u,
        42043u,      45481u,      49201u,      53201u,      57557u,      62233u,      67307u,      72817u,      78779u,      85229u,      92203u,      99733u,      107897u,     116731u,     126271u,     136607u,     147793u,     159871u,
        172933u,     187091u,     202409u,     218971u,     236897u,     256279u,     277261u,     299951u,     324503u,     351061u,     379787u,     410857u,     444487u,     480881u,     520241u,     562841u,     608903u,     658753u,
        712697u,     771049u,     834181u,     902483u,     976369u,     1056323u,    1142821u,    1236397u,    1337629u,    1447153u,    1565659u,    1693859u,    1832561u,    1982627u,    2144977u,    2320627u,    2510653u,    2716249u,
        2938679u,    3179303u,    3439651u,    3721303u,    4026031u,    4355707u,    4712381u,    5098259u,    5515729u,    5967347u,    6456007u,    6984629u,    7556579u,    8175383u,    8844859u,    9569143u,    10352717u,   11200489u,
        12117689u,   13109983u,   14183539u,   15345007u,   16601593u,   17961079u,   19431899u,   21023161u,   22744717u,   24607243u,   26622317u,   28802401u,   31160981u,   33712729u,   36473443u,   39460231u,   42691603u,   46187573u,
        49969847u,   54061849u,   58488943u,   63278561u,   68460391u,   74066549u,   80131819u,   86693767u,   93793069u,   101473717u,  109783337u,  118773397u,  128499677u,  139022417u,  150406843u,  162723577u,  176048909u,  190465427u,
        206062531u,  222936881u,  241193053u,  260944219u,  282312799u,  305431229u,  330442829u,  357502601u,  386778277u,  418451333u,  452718089u,  489790921u,  529899637u,  573292817u,  620239453u,  671030513u,  725980837u,  785430967u,
        849749479u,  919334987u,  994618837u,  1076067617u, 1164186217u, 1259520799u, 1362662261u, 1474249943u, 1594975441u, 1725587117u, 1866894511u, 2019773507u, 2185171673u, 2364114217u, 2557710269u, 2767159799u, 2993761039u, 3238918481u,
        3504151727u, 3791104843u, 4101556399u, 4294967291u,
        4294967291u // Sentinel so we don't have to test result of lower_bound
    };
    // NOLINTEND

    constexpr card32 g_prime_count            = rsl::size(g_prime_number_array) - 1;
    constexpr float32 g_default_growth_factor = 2.0f;

    prime_rehash_policy::prime_rehash_policy(float32 maxLoadFactor)
        : m_max_load_factor(maxLoadFactor)
        , m_growth_factor(g_default_growth_factor)
        , m_next_resize(0)
    {
    }

    float32 prime_rehash_policy::get_max_load_factor() const
    {
      return m_max_load_factor;
    }

    uint32 prime_rehash_policy::get_prev_bucket_count_only(uint32 bucketCountHint)
    {
      const uint32 prime = *(rsl::upper_bound(g_prime_number_array, g_prime_number_array + g_prime_count, bucketCountHint) - 1);
      return prime;
    }

    uint32 prime_rehash_policy::get_prev_bucket_count(uint32 bucketCountHint) const
    {
      const uint32 prime = *(rsl::upper_bound(g_prime_number_array, g_prime_number_array + g_prime_count, bucketCountHint) - 1);

      m_next_resize = static_cast<uint32>(ceil(prime * m_max_load_factor)); // NOLINT
      return prime;
    }

    uint32 prime_rehash_policy::get_next_bucket_count(uint32 bucketCountHint) const
    {
      const uint32 prime = *rsl::lower_bound(g_prime_number_array, g_prime_number_array + g_prime_count, bucketCountHint);

      m_next_resize = static_cast<uint32>(ceil(prime * m_max_load_factor)); // NOLINT
      return prime;
    }

    uint32 prime_rehash_policy::get_bucket_count(uint32 elementCount) const
    {
      const uint32 min_bucket_count = static_cast<uint32>(elementCount / m_max_load_factor); // NOLINT
      const uint32 prime            = *rsl::lower_bound(g_prime_number_array, g_prime_number_array + g_prime_count, elementCount);

      m_next_resize = static_cast<uint32>(ceil(prime * m_max_load_factor)); // NOLINT
      return prime;
    }

    hash_required_result prime_rehash_policy::is_rehash_required(uint32 bucketCount, uint32 elementCount, uint32 elementAdd) const
    {
      if((elementCount + elementAdd) > m_next_resize)
      {
        if(bucketCount == 1)
        {
          bucketCount = 0;
        }

        float32 min_bucket_count = (elementCount + elementAdd) / m_max_load_factor; // NOLINT

        if(min_bucket_count > static_cast<float32>(bucketCount))
        {
          min_bucket_count    = (rsl::max)(min_bucket_count, m_growth_factor * bucketCount); // NOLINT
          const count_t prime = static_cast<count_t>(*rsl::lower_bound(g_prime_number_array, g_prime_number_array + g_prime_count, static_cast<uint32>(min_bucket_count)));
          m_next_resize       = static_cast<uint32>(ceil(prime * m_max_load_factor)); // NOLINT

          return hash_required_result {prime, true};
        }
        else
        {
          m_next_resize = static_cast<uint32>(ceil(bucketCount * m_max_load_factor)); // NOLINT
          return hash_required_result {0, false};
        }
      }

      return hash_required_result {0, false};
    }

  } // namespace v1
} // namespace rsl