// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: time_point.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/internal/chrono/duration.h"
#include "rex_std/internal/type_traits/common_type.h"
#include "rex_std/internal/type_traits/enable_if.h"
#include "rex_std/internal/type_traits/is_arithmetic.h"

namespace rsl
{
  inline namespace v1
  {
    namespace chrono
    {
      template <typename Clock, typename Duration = typename Clock::duration>
      class time_point
      {
      public:
        using clock    = Clock;
        using duration = Duration;
        using rep      = typename duration::rep;
        using period   = typename duration::period;

        constexpr time_point()
            : m_duration(duration::zero())
        {
        }
        explicit time_point(const duration& other)
            : m_duration(other)
        {
        }
        template <typename Duration2>
        constexpr explicit time_point(const time_point<Clock, Duration2>& t, enable_if_t<is_convertible_v<Duration2, duration>>** /*unused*/ = nullptr)
            : m_duration(t.time_since_epoch())
        {
        }

        constexpr duration time_since_epoch() const
        {
          return m_duration;
        }

        time_point& operator+=(const duration& d)
        {
          m_duration += d;
          return *this;
        }
        time_point& operator-=(const duration& d)
        {
          m_duration -= d;
          return *this;
        }

        static constexpr time_point(min)()
        {
          return time_point((duration::min)());
        }
        static constexpr time_point(max)()
        {
          return time_point((duration::max)());
        }

      private:
        duration m_duration;
      };

      template <typename Clock, typename Duration1, typename Rep2, typename Period2>
      constexpr time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type> operator+(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_timepoint = time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type>;
        return common_timepoint(lhs.time_since_epoch() + rhs);
      }

      template <class Rep1, class Period1, typename Clock, class Duration2>
      constexpr time_point<Clock, typename common_type<Duration2, duration<Rep1, Period1>>::type> operator+(const duration<Rep1, Period1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        using common_timepoint = time_point<Clock, typename common_type<Duration2, duration<Rep1, Period1>>::type>;
        return common_timepoint(lhs + rhs.time_since_epoch());
      }

      template <typename Clock, typename Duration1, typename Rep2, typename Period2>
      constexpr time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type> operator-(const time_point<Clock, Duration1>& lhs, const duration<Rep2, Period2>& rhs)
      {
        using common_timepoint = time_point<Clock, typename common_type<Duration1, duration<Rep2, Period2>>::type>;
        return common_timepoint(lhs.time_since_epoch() - rhs);
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr typename common_type<Duration1, Duration2>::type operator-(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return lhs.time_since_epoch() - rhs.time_since_epoch();
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator==(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return lhs.time_since_epoch() == rhs.time_since_epoch();
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator!=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return !(lhs == rhs);
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator<(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return lhs.time_since_epoch() < rhs.time_since_epoch();
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator<=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return !(rhs < lhs);
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator>(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return rhs < lhs;
      }

      template <typename Clock, typename Duration1, class Duration2>
      constexpr bool operator>=(const time_point<Clock, Duration1>& lhs, const time_point<Clock, Duration2>& rhs)
      {
        return !(lhs < rhs);
      }

      template <typename ToDuration, typename Clock, typename Duration>
      constexpr time_point<Clock, ToDuration> time_point_cast(const time_point<Clock, Duration>& t, typename enable_if<internal::is_duration<ToDuration>::value>::type** /*unused*/ = nullptr)
      {
        return time_point<Clock, ToDuration>(duration_cast<ToDuration>(t.time_since_epoch()));
      }

      template <typename ToDuration, typename Clock, typename Duration, enable_if_t<internal::is_duration<ToDuration>::value, bool> = true>
      REX_NO_DISCARD constexpr time_point<Clock, ToDuration> floor(const time_point<Clock, Duration>& time)
      {
        // change the duration type of a time_point; round towards negative infinity
        return time_point<Clock, ToDuration>(rsl::chrono::floor<ToDuration>(time.time_since_epoch()));
      }

      template <typename ToDuration, typename Clock, typename Duration, enable_if_t<internal::is_duration<ToDuration>::value, bool> = true>
      REX_NO_DISCARD constexpr time_point<Clock, ToDuration> ceil(const time_point<Clock, Duration>& time)
      {
        // change the duration type of a time_point; round towards positive infinity
        return time_point<Clock, ToDuration>(rsl::chrono::ceil<ToDuration>(time.time_since_epoch()));
      }

      template <typename ToDuration, typename Clock, typename Duration, enable_if_t<internal::is_duration<ToDuration>::value && !treat_as_floating_point<typename ToDuration::rep>::value, bool> = true>
      REX_NO_DISCARD constexpr time_point<Clock, ToDuration> round(const time_point<Clock, Duration>& time)
      {
        // change the duration type of a time_point; round to nearest, ties to even
        return time_point<Clock, ToDuration>(rsl::chrono::round<ToDuration>(time.time_since_epoch()));
      }

    } // namespace chrono

    template <typename Clock, typename Duration1, typename Duration2>
    struct common_type<chrono::time_point<Clock, Duration1>, chrono::time_point<Clock, Duration2>>
    {
      using type = rsl::chrono::time_point<Clock, common_type_t<Duration1, Duration2>>;
    };

  } // namespace v1
};  // namespace rsl
