// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: duration.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/string/string_utils.h"
#include "rex_std/bonus/types.h"
#include "rex_std/internal/assert/assert_fwd.h"
#include "rex_std/internal/chrono/duration_values.h"
#include "rex_std/internal/chrono/treat_as_floating_point.h"
#include "rex_std/internal/type_traits/common_type.h"
#include "rex_std/internal/type_traits/decay.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/integral_constant.h"
#include "rex_std/internal/type_traits/is_convertible.h"
#include "rex_std/internal/type_traits/remove_cvref.h"
#include "rex_std/limits.h"
#include "rex_std/ostream.h"
#include "rex_std/ratio.h"

namespace rsl
{
  inline namespace v1
  {
    namespace chrono
    {
      template <typename Rep, typename Period = ratio<1>>
      class duration;

      namespace internal
      {
        template <typename>
        struct is_duration : false_type
        {
        };
        template <typename Rep, typename Period>
        struct is_duration<duration<Rep, Period>> : true_type
        {
        };
        template <typename Rep, typename Period>
        struct is_duration<const duration<Rep, Period>> : true_type
        {
        };
        template <typename Rep, typename Period>
        struct is_duration<volatile duration<Rep, Period>> : true_type
        {
        };
        template <typename Rep, typename Period>
        struct is_duration<const volatile duration<Rep, Period>> : true_type
        {
        };

        template <typename Period1, typename Period2>
        struct ratio_gcd
        {
          static_assert(rsl::internal::is_ratio<Period1>, "Period1 is not a rsl::ratio type");
          static_assert(rsl::internal::is_ratio<Period2>, "Period2 is not a rsl::ratio type");

          using type = ratio<rsl::internal::gcd<Period1::num, Period2::num>::value, rsl::internal::lcm<Period1::den, Period2::den>::value>;
        };

        // duration cast
        template <typename FromDuration, typename ToDuration, typename CommonPeriod = typename RatioDivide<typename FromDuration::period, typename ToDuration::period>::type,
                  typename CommonRep = typename rsl::decay_t<common_type_t<typename ToDuration::rep, typename FromDuration::rep, intmax>>, bool = CommonPeriod::num == 1, bool = CommonPeriod::den == 1>
        struct duration_cast_impl;

        template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
        struct duration_cast_impl<FromDuration, ToDuration, CommonPeriod, CommonRep, true, true>
        {
          static ToDuration do_cast(const FromDuration& fd)
          {
            return ToDuration(static_cast<typename ToDuration::rep>(fd.count()));
          }
        };

        template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
        struct duration_cast_impl<FromDuration, ToDuration, CommonPeriod, CommonRep, false, true>
        {
          static ToDuration do_cast(const FromDuration& d)
          {
            return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) * static_cast<CommonRep>(CommonPeriod::num)));
          }
        };

        template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
        struct duration_cast_impl<FromDuration, ToDuration, CommonPeriod, CommonRep, true, false>
        {
          static ToDuration do_cast(const FromDuration& d)
          {
            return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) / static_cast<CommonRep>(CommonPeriod::den)));
          }
        };

        template <typename FromDuration, typename ToDuration, typename CommonPeriod, typename CommonRep>
        struct duration_cast_impl<FromDuration, ToDuration, CommonPeriod, CommonRep, false, false>
        {
          static ToDuration do_cast(const FromDuration& d)
          {
            return ToDuration(static_cast<typename ToDuration::rep>(static_cast<CommonRep>(d.count()) * static_cast<CommonRep>(CommonPeriod::num) / static_cast<CommonRep>(CommonPeriod::den)));
          }
        };
      } // namespace internal

      template <typename ToDuration, typename Rep, typename Period, enable_if_t<internal::is_duration<ToDuration>::value, bool> Enabled = true>
      ToDuration duration_cast(const duration<Rep, Period>& d)
      {
        using FromDuration = rsl::remove_cvref_t<decltype(d)>;
        return internal::duration_cast_impl<FromDuration, ToDuration>::do_cast(d);
      }

      template <typename Rep, typename Period>
      class duration
      {
      private:
        using this_type = duration<Rep, Period>;

      public:
        using rep    = Rep;
        using period = Period;

        constexpr duration()      = default;
        duration(const duration&) = default;
        ~duration()               = default;

        template <typename Rep2>
        constexpr explicit duration(const Rep2& rep2, enable_if_t<is_convertible_v<Rep2, Rep> && (treat_as_floating_point<Rep>::value || !treat_as_floating_point<Rep2>::value)>** /*unused*/ = nullptr)
            : m_rep(static_cast<Rep>(rep2))
        {
        }

        template <typename Rep2, typename Period2>
        constexpr explicit duration(const duration<Rep2, Period2>& d2, enable_if_t<treat_as_floating_point<Rep>::value || (RatioDivide<Period2, Period>::den == 1 && !treat_as_floating_point<Rep2>::value), void>** /*unused*/ = nullptr)
            : m_rep(duration_cast<duration>(d2).count())
        {
        }

        constexpr duration& operator=(const duration&) = default;

        constexpr Rep count() const
        {
          return m_rep;
        }

        constexpr static duration zero()
        {
          return duration(duration_values<Rep>::zero());
        }
        constexpr static duration(min)()
        {
          return duration((duration_values<Rep>::min)());
        }
        constexpr static duration(max)()
        {
          return duration((duration_values<Rep>::max)());
        }

        constexpr common_type<duration> operator+() const
        {
          return *this;
        }
        constexpr common_type<duration> operator-() const
        {
          return duration(0 - m_rep);
        }

        duration operator++(int)
        {
          return duration(m_rep++);
        }
        duration operator--(int)
        {
          return duration(m_rep--);
        }
        duration& operator++()
        {
          ++m_rep;
          return *this;
        }
        duration& operator--()
        {
          --m_rep;
          return *this;
        }
        duration& operator+=(const duration& d)
        {
          m_rep += d.count();
          return *this;
        }
        duration& operator-=(const duration& d)
        {
          m_rep -= d.count();
          return *this;
        }
        duration& operator*=(const Rep& rhs)
        {
          m_rep *= rhs;
          return *this;
        }
        duration& operator/=(const Rep& rhs)
        {
          m_rep /= rhs;
          return *this;
        }
        duration& operator%=(const Rep& rhs)
        {
          m_rep %= rhs;
          return *this;
        }
        duration& operator%=(const duration& d)
        {
          m_rep %= d.count();
          return *this;
        }

      private:
        Rep m_rep;
      };

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> operator+(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_duration = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
        return common_duration(common_duration(lhs).count() + common_duration(rhs).count());
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>> operator-(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_duration = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
        return common_duration(common_duration(lhs).count() - common_duration(rhs).count());
      }

      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator*(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(common_duration(lhs).count() * rhs);
      }
      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator*(const Rep2& lhs, const duration<Rep1, Period1>& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(lhs * common_duration(rhs).count());
      }

      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator/(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(common_duration(lhs).count() / rhs);
      }
      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator/(const Rep2& lhs, const duration<Rep1, Period1>& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(lhs / common_duration(rhs).count());
      }

      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator%(const duration<Rep1, Period1>& lhs, const Rep2& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(common_duration(lhs).count() % rhs);
      }
      template <typename Rep1, typename Period1, typename Rep2>
      duration<common_type_t<Rep1, Rep2>, Period1> operator%(const Rep2& lhs, const duration<Rep1, Period1>& rhs)
      {
        using common_duration = duration<common_type_t<Rep1, Rep2>, Period1>;
        return common_duration(lhs % common_duration(rhs).count());
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator==(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_duration = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
        return common_duration(lhs).count() == common_duration(rhs).count();
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator<(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_duration = common_type_t<duration<Rep1, Period1>, duration<Rep2, Period2>>;
        return common_duration(lhs).count() < common_duration(rhs).count();
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator!=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        return !(lhs == rhs);
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator<=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        return !(rhs < lhs);
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator>(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        return rhs < lhs;
      }

      template <typename Rep1, typename Period1, typename Rep2, typename Period2>
      bool operator>=(const duration<Rep1, Period1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        return !(lhs < rhs);
      }

      using nanoseconds  = duration<int64, nano>;
      using microseconds = duration<int64, micro>;
      using milliseconds = duration<int64, milli>;
      using seconds      = duration<int64>;
      using minutes      = duration<int64, ratio<60>>;
      using hours        = duration<int64, ratio<3600>>;
      using days         = duration<int64, ratio<86400>>;
      using weeks        = duration<int64, ratio<604800>>;
      using months       = duration<int64, ratio<2629746>>;
      using years        = duration<int64, ratio<31556952>>;

      template <typename ToDuration, typename Rep, typename Period, enable_if_t<internal::is_duration<ToDuration>::value, bool> = true>
      REX_NO_DISCARD constexpr ToDuration floor(const duration<Rep, Period>& dur)
      {
        // convert duration to another duration; round towards negative infinity
        // i.e. the greatest integral result such that the result <= dur
        const ToDuration casted {chrono::duration_cast<ToDuration>(dur)};
        if(casted > dur)
        {
          return ToDuration {casted.count() - static_cast<typename ToDuration::rep>(1)};
        }

        return casted;
      }

      template <typename ToDuration, typename Rep, typename Period, enable_if_t<internal::is_duration<ToDuration>::value, bool> = true>
      REX_NO_DISCARD constexpr ToDuration ceil(const duration<Rep, Period>& dur)
      {
        // convert duration to another duration; round towards positive infinity
        // i.e. the least integral result such that dur <= the result
        const ToDuration casted {chrono::duration_cast<ToDuration>(dur)};
        if(casted < dur)
        {
          return ToDuration {casted.count() + static_cast<typename ToDuration::rep>(1)};
        }

        return casted;
      }

      namespace internal
      {
        template <typename Rep>
        constexpr bool is_even(Rep val)
        {
          // Tests whether val is even
          return val % 2 == 0;
        }
      } // namespace internal

      template <typename ToDuration, typename Rep, typename Period, enable_if_t<internal::is_duration<ToDuration>::value && !treat_as_floating_point<typename ToDuration::rep>::value, bool> = true>
      REX_NO_DISCARD constexpr ToDuration round(const duration<Rep, Period>& dur)
      {
        // convert duration to another duration, round to nearest, ties to even
        const ToDuration floored {chrono::floor<ToDuration>(dur)};
        const ToDuration ceiled {floored + ToDuration {1}};
        const auto floor_adjustment = dur - floored;
        const auto ceil_adjustment  = ceiled - dur;
        if(floor_adjustment < ceil_adjustment || (floor_adjustment == ceil_adjustment && internal::is_even(floored.count())))
        {
          return floored;
        }

        return ceiled;
      }

      template <typename Rep, typename Period, enable_if_t<numeric_limits<Rep>::is_signed, bool> = true>
      REX_NO_DISCARD constexpr duration<Rep, Period> abs(const duration<Rep, Period> dur)
      {
        // create a duration with count() the absolute value of dur.count()
        return dur < duration<Rep, Period>::zero() ? duration<Rep, Period>::zero() - dur : dur;
      }

      namespace internal
      {
#define IF_PERIOD_RETURN_SUFFIX_ELSE(type, suffix)                                                                                                                                                                                                       \
  if constexpr(is_same_v<Period, type>)                                                                                                                                                                                                                  \
  {                                                                                                                                                                                                                                                      \
    if constexpr(is_same_v<CharType, char8>)                                                                                                                                                                                                             \
    {                                                                                                                                                                                                                                                    \
      return suffix;                                                                                                                                                                                                                                     \
    }                                                                                                                                                                                                                                                    \
    else                                                                                                                                                                                                                                                 \
    {                                                                                                                                                                                                                                                    \
      return L##suffix;                                                                                                                                                                                                                                  \
    }                                                                                                                                                                                                                                                    \
  }

        template <typename CharType, typename Period>
        constexpr const CharType* get_literal_unit_suffix() // NOLINT(readability-function-cognitive-complexity)
        {
          IF_PERIOD_RETURN_SUFFIX_ELSE(atto, "as")
          IF_PERIOD_RETURN_SUFFIX_ELSE(femto, "fs")
          IF_PERIOD_RETURN_SUFFIX_ELSE(pico, "ps")
          IF_PERIOD_RETURN_SUFFIX_ELSE(nano, "ns")
          IF_PERIOD_RETURN_SUFFIX_ELSE(micro, "us")
          IF_PERIOD_RETURN_SUFFIX_ELSE(milli, "ms")
          IF_PERIOD_RETURN_SUFFIX_ELSE(centi, "cs")
          IF_PERIOD_RETURN_SUFFIX_ELSE(deci, "ds")
          IF_PERIOD_RETURN_SUFFIX_ELSE(seconds::period, "s")
          IF_PERIOD_RETURN_SUFFIX_ELSE(deca, "das")
          IF_PERIOD_RETURN_SUFFIX_ELSE(hecto, "hs")
          IF_PERIOD_RETURN_SUFFIX_ELSE(kilo, "ks")
          IF_PERIOD_RETURN_SUFFIX_ELSE(mega, "Ms")
          IF_PERIOD_RETURN_SUFFIX_ELSE(giga, "Gs")
          IF_PERIOD_RETURN_SUFFIX_ELSE(tera, "Ts")
          IF_PERIOD_RETURN_SUFFIX_ELSE(peta, "Ps")
          IF_PERIOD_RETURN_SUFFIX_ELSE(exa, "Es")
          IF_PERIOD_RETURN_SUFFIX_ELSE(minutes::period, "min")
          IF_PERIOD_RETURN_SUFFIX_ELSE(hours::period, "h")
          IF_PERIOD_RETURN_SUFFIX_ELSE(ratio<86400>, "d")

          return nullptr;
        }

        template <typename CharType>
        CharType* get_general_unit_suffix(CharType* rnext, const intmax num, const intmax den)
        {
          REX_ASSERT_X(num > 0 && den > 0, "num and den must be above 0");
          *--rnext = '\0';
          *--rnext = 's';
          *--rnext = ']';
          if(den != 1)
          {
            rnext    = rsl::internal::unsigned_integral_to_buff(rnext, static_cast<uintmax>(den));
            *--rnext = '/';
          }

          rnext    = rsl::internal::unsigned_integral_to_buff(rnext, static_cast<uintmax>(num));
          *--rnext = '[';
          return rnext;
        }

#undef IF_PERIOD_RETURN_SUFFIX_ELSE
      } // namespace internal

      // template <typename CharType, typename Traits>
      // class basic_ostream;

      template <typename CharType, typename Traits, typename Rep, typename Period>
      basic_ostream<CharType, Traits>& operator<<(basic_ostream<CharType, Traits>& os, const duration<Rep, Period>& dur)
      {
        basic_ostream<CharType, Traits> ss;
        ss << dur.count();

        constexpr auto suffix = internal::get_literal_unit_suffix<CharType, Period>();
        if constexpr(suffix == nullptr)
        {
          CharType buffer[2 * (numeric_limits<intmax>::digits10 + 1) + 5] = {}; // 2 numbers + "[/]s\0"
          const CharType* const begin                                     = internal::get_general_unit_suffix<CharType>(rsl::end(buffer), Period::num, Period::den);
          ss << begin;
        }
        else
        {
          ss << suffix;
        }

        return os << ss.str();
      }

      /// [24/Jul/2022] RSL TODO: We have yet to implement rsl::chrono::from_stream
      // template <typename CharType, typename Traits, typename Rep, typename Period, typename Alloc = rsl::allocator<CharType>>
      // basic_istream<CharType, Traits>& from_stream(basic_istream<CharType, Traits>& is, const CharType* fmt,
      // duration<Rep, Period>& d, basic_string<CharType, Traits, Alloc>* abbrev = nullptr, minutes* offset = nullptr);

      /// [24/Jul/2022] RSL TODO: We have yet to implement rsl::chrono::Formatter
      // template <typename Rep, typename Period, typename CharType>
      // struct Formatter<Chrono::duration<Rep, Period>, CharType>;
    } // namespace chrono

    template <typename Rep1, typename Period1, typename Rep2, typename Period2>
    struct common_type<chrono::duration<Rep1, Period1>, chrono::duration<Rep2, Period2>>
    {
      using type = chrono::duration<decay_t<common_type_t<Rep1, Rep2>>, typename internal::ratio_gcd<Period1, Period2>::type>;
    };

  } // namespace v1
} // namespace rsl

namespace rsl
{
  inline namespace v1
  {
    namespace chrono_literals
    {
#if defined(REX_MSVC_COMPILER)
  #pragma warning(push)
  #pragma warning(disable : 4455) // literal suffix identifiers that do not start with an underscore are reserved
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic push
  #pragma clang diagnostic ignored "-Wuser-defined-literals" // literal suffix identifiers that do not start with an underscore are reserved
#endif
      constexpr rsl::chrono::hours operator"" h(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::hours(val);
      }
      constexpr rsl::chrono::duration<float64, ratio<3600>> operator"" h(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64, ratio<3600>>(val);
      }
      constexpr rsl::chrono::minutes(operator"" min)(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::minutes(val);
      }
      constexpr rsl::chrono::duration<float64, ratio<60>>(operator"" min)(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64, ratio<60>>(val);
      }
      constexpr rsl::chrono::seconds operator"" s(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::seconds(val);
      }
      constexpr rsl::chrono::duration<float64> operator"" s(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64>(val);
      }
      constexpr rsl::chrono::milliseconds operator"" ms(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::milliseconds(val);
      }
      constexpr rsl::chrono::duration<float64, milli> operator"" ms(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64, milli>(val);
      }
      constexpr rsl::chrono::microseconds operator"" us(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::microseconds(val);
      }
      constexpr rsl::chrono::duration<float64, micro> operator"" us(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64, micro>(val);
      }
      constexpr rsl::chrono::nanoseconds operator"" ns(uint64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::nanoseconds(val);
      }
      constexpr rsl::chrono::duration<float64, nano> operator"" ns(lfloat64 val) // NOLINT(clang-diagnostic-user-defined-literals)
      {
        return rsl::chrono::duration<float64, nano>(val);
      }
#if defined(REX_MSVC_COMPILER)
  #pragma warning(pop)
#elif defined(REX_CLANG_COMPILER)
  #pragma clang diagnostic pop
#endif
    } // namespace chrono_literals
  }   // namespace v1
} // namespace rsl
