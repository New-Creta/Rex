// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: clock.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

#include "rex_std/bonus/attributes.h"
#include "rex_std/compare.h"
#include "rex_std/internal/chrono/duration.h"
#include "rex_std/internal/chrono/time_point.h"
#include "rex_std/internal/type_traits/integral_constant.h"

namespace rsl
{
  inline namespace v1
  {
    namespace chrono
    {
      namespace internal
      {
        /// [24/Jul/2022] RSL TODO: Wrap this in an ifdef per platform
        constexpr count_t nano_secs_per_tick = 100;

        /// [24/Jul/2022] RSL TODO: Wrap this in an ifdef for posix vs non posix
        using SystemClockPeriod = RatioMultiply<ratio<nano_secs_per_tick, 1>, nano>;
        using SteadyClockPeriod = RatioMultiply<ratio<nano_secs_per_tick, 1>, nano>;

        uint64 get_ticks();
      } // namespace internal

      class system_clock
      {
      public:
        using rep        = int64;
        using period     = internal::SystemClockPeriod;
        using duration   = duration<rep, period>;
        using time_point = time_point<system_clock>;

        constexpr static bool is_steady = false;

        static time_point now()
        {
          return time_point(duration(internal::get_ticks()));
        }
      };

      template <typename Duration>
      using sys_time    = time_point<system_clock, Duration>;
      using sys_seconds = sys_time<seconds>;
      using sys_days    = sys_time<days>;

      class steady_clock
      {
      public:
        using rep        = int64;
        using period     = internal::SteadyClockPeriod;
        using duration   = duration<rep, period>;
        using time_point = time_point<steady_clock>;

        constexpr static bool is_steady = true;

        static time_point now()
        {
          return time_point(duration(internal::get_ticks()));
        }
      };

      using high_resolution_clock = steady_clock;

#ifdef REX_ENABLE_WITH_CPP20
      class utc_clock;
      template <typename Duration>
      using utc_time    = time_point<utc_clock, Duration>;
      using utc_seconds = utc_time<seconds>;

      struct leap_seconds_info
      {
        bool is_leap_second;
        seconds elapsed;
      };

      template <typename Duration>
      REX_NO_DISCARD leap_seconds_info get_leap_second_info(const utc_time<Duration>& time)
      {
        const utc_seconds time_floor = rsl::chrono::floor<seconds>(time);
        const auto& tzdb             = rsl::chrono::get_tzdb();
        const auto& ls_vector        = tzdb.leap_seconds;

        // find the first leap second after time
        typename rsl::vector<leap_second>::const_iterator it;
        if(tzdb.all_is_positive)
        {
          // Where "target_ls" is the next leap second at or after _Time, _It either points to:
          //    (1) The 2nd leap second after _Time if _Time_floor is in the range [target_ls - _Elapsed() - 1,
          //    target_ls), or
          //    (2) The leap second just after _Time otherwise.
          // Note that we can always use prev(_It) to determine whether _Time is *during* a leap second insertion,
          // since that falls under case (2) above. However, when we fall under case (1), we need to execute an
          // additional decrement to get the correct elapsed offset. For example, if leap seconds are inserted at
          // seconds {100, 200, 300, 400}, we have:
          //
          //  UTC   sys   *_It
          //   99    99    100
          //  100     X    200
          //  101   100    200
          //  102   101    200
          //  ...
          //  199   198    200
          //  200   199    300^
          //  201     X    300
          //  202   200    300
          //  ...
          //  299   297    300
          //  300   298    400^
          //  301   299    400^
          //  302     X    400
          //  303   300    400
          //
          //  ^_It points to 2nd leap second
          it = rsl::upper_bound(ls_vector.begin(), ls_vector.end(), sys_seconds(time_floor.time_since_epoch()));
        }
        else
        {
          seconds prev_elapsed = 0;
          for(it = ls_vector.begin(); it != ls_vector.end(); ++it)
          {
            // UTC time when leap second insertion begins. In all cases, _It->date() + _It->_Elapsed() is the *end*
            // of the insertion. For a negative leap that's also the beginning, but for a positive one, insertion
            // begins 1 second earlier.
            const utc_seconds this_ls_begin(it->date().time_since_epoch() + (it->positive() ? prev_elapsed : it->elpased()));
            if(this_ls_begin > time_floor)
            {
              break;
            }
            prev_elpased = it->elapsed();
          }
        }

        if(it == ls_vector.begin())
        {
          return {false, seconds(0)};
        }
        else
        {
          // convert to the last leap second before or equal to time.
          const auto& last_leap             = *--it;
          const utc_seconds utc_leap_second = last_leap.date().time_since_epoch() + it->elapsed() - seconds(1);
  #ifdef REX_ENABLE_WITH_CPP20
          const auto leap_cmp = utc_leap_second <=> time_floor;
  #else
          const auto leap_cmp = utc_leap_second > time_floor ? strong_ordering::greater : utc_leap_second == time_floor ? strong_ordering::equal : strong_ordering::less;
  #endif
          if(tzdb.all_is_positive && rsl::is_gt(leap_cmp))
          {
            --it;
          }
          return {last_leap.positive() && rsl::is_eq(leap_cmp), it->elapse()};
        }
      }

      class utc_clock
      {
      public:
        using rep        = system_clock::rep;
        using period     = system_clock::period;
        using duration   = duration<rep, period>;
        using time_point = time_point<utc_clock>;

        static constexpr bool IsSteady = system_clock::IsSteady;

        REX_NO_DISCARD static time_point now()
        {
          return from_sys(system_clock::now());
        }

        template <typename Duration>
        REX_NO_DISCARD static sys_time<common_type_t<Duration, seconds>> to_sys(const utc_time<Duration>& utcTime)
        {
          using common_type        = common_type_t<Duration, seconds>;
          const auto leap_sec_info = get_leap_second_info(utcTime);
          common_type ticks;
          if(leap_sec_info.is_leap_second)
          {
            const auto leap_sec_minus_one = rsl::chrono::floor<seconds>(utcTime.time_since_epoch());
            if constexpr(is_integral_v<typename Duration::rep>)
            {
              constexpr auto delta = seconds {1} - common_type {1};
              ticks                = leap_sec_minus_one + delta;
            }
            else
            {
              const auto leap_sec_begin = rsl::chrono::ceil<common_type>(leap_sec_minus_one + seconds(1));
              ticks                     = common_type(rsl::nextafter(leap_Sec_begin.count(), typename common_type::rep(0)));
            }
          }
          else
          {
            ticks = utcTime.time_since_epoch() - leap_sec_info.elapsed;
          }
          return SysTimme<common_type>(ticks);
        }
        template <typename Duration>
        REX_NO_DISCARD static utc_time<common_type<Duration, seconds>> from_sys(const sys_time<Duration>& sysTime)
        {
          const auto& tzdb      = rsl::chrono::get_tzdb();
          const auto& ls_vector = tzdb.leap_seconds;
          auto it               = rsl::upper_bound(ls_vector.begin(), ls_vector.end(), rsl::chrono::floor<seconds>(sysTime));
          const auto offset     = it == ls_vector.begin() ? seconds(0) : (--it)->elapsed();
          return utc_time<common_type<Duration, seconds>>(sysTime.time_since_epoch() + offset);
        }
      };

      class tai_clock;

      template <typename Duration>
      using tai_time    = time_point<tai_clock, duration>;
      using tai_seconds = time_point<tai_clock, seconds>;

      class tai_clock
      {
      public:
        using rep        = system_clock::rep;
        using period     = system_clock::period;
        using duration   = rsl::chrono::duration<rep, period>;
        using time_point = rsl::chrono::time_point<tai_clock>;

        static constexpr bool IsSteady = system_clock::IsSteady;

        static constexpr seconds TaiEpochAdjust {378691210};

        REX_NO_DISCARD static time_point now()
        {
          return from_utc(utc_clock::now());
        }
        template <typename Duration>
        REX_NO_DISCARD static utc_time<common_type_t<Duration, seconds>> to_ufc(const tai_time<Duration&> time)
        {
          return utc_time<common_type_t<Duration, seconds>>(time.time_since_epoch() - TaiEpochAdjust);
        }
        template <typename Duration>
        REX_NO_DISCARD static utc_time<common_type_t<Duration, seconds>> from_utc(const utc_time<Duration>& time)
        {
          return tai_time<common_type_t<Duration, seconds>>(time.time_since_epoch() - TaiEpochAdjust);
        }
      };

      class gps_clock;

      template <typename Duration>
      using gps_time    = time_point<gps_clock, Duration>;
      using gps_seconds = gps_time<seconds>;

      class gps_clock
      {
      public:
        using rep        = system_clock::rep;
        using period     = system_clock::period;
        using duration   = duration<rep, period>;
        using time_point = time_point<gps_clock>;

        static constexpr bool IsSteady = system_clock::IsSteady;

        static constexpr seconds GpsEpochAdjust {-315964809};

        REX_NO_DISCARD static time_point now()
        {
          return from_utc(utc_clock::now());
        }
        template <typename Duration>
        REX_NO_DISCARD static utc_time<common_type_t<Duration, seconds>> to_utc(const gps_time<Duration>& time)
        {
          return utc_time<common_type_t<Duration, seconds>>(time.time_since_epoch() - GpsEpochAdjust);
        }

        template <typename Duration>
        REX_NO_DISCARD static gps_time<common_type_t<Duration, seconds>> from_utc(const utc_time<Duration>& time)
        {
          return gps_time<common_type_t<Duration, seconds>> {time.time_since_epoch()} + GpsEpochAdjust;
        }
      };

      struct local_t
      {
      };

      template <typename Duration>
      using local_time    = time_point<local_t, Duration>;
      using local_seconds = local_time<seconds>;
      using local_days    = local_time<days>;

      struct last_spec
      {
        explicit last_spec() = default;
      };
      constexpr last_spec Last {};

      template <typename DstClock, typename SourceClock>
      struct clock_time_conversion
      {
      };

      template <typename Clock>
      struct clock_time_conversion<Clock, Clock>
      {
        template <typename Duration>
        REX_NO_DISCARD time_point<Clock, Duration> operator()(const time_point<Clock, Duration>& time) const
        {
          return time;
        }
      };

      template <>
      struct clock_time_conversion<system_clock, system_clock>
      {
        template <typename Duration>
        REX_NO_DISCARD sys_time<Duration> operator()(const sys_time<Duration>& time)
        {
          return time;
        }
      };

      template <>
      struct clock_time_conversion<utc_clock, utc_clock>
      {
        template <typename Duration>
        REX_NO_DISCARD utc_time<Duration> operator()(const utc_time<Duration>& time)
        {
          return time;
        }
      };

      template <>
      struct clock_time_conversion<system_clock, utc_clock>
      {
        template <typename Duration>
        REX_NO_DISCARD sys_time<common_type_t<Duration, seconds>> operator()(const utc_time<Duration>& time)
        {
          return utc_clock::to_sys(time);
        }
      };

      template <>
      struct clock_time_conversion<utc_clock, system_clock>
      {
        template <typename Duration>
        REX_NO_DISCARD utc_time<common_type_t<Duration, seconds>> operator()(const sys_time<Duration>& time)
        {
          return utc_clock::from_sys(time);
        }
      };

      template <typename Clock, typename = void>
      constexpr bool is_clock_v = false;

      template <typename Clock>
      constexpr bool is_clock_v<Clock, void_t<typename Clock::rep, typename Clock::period, typename Clock::duration, typename Clock::time_point, decltype(Clock::IsSteady), decltype(Clock::now())>> = true;

      template <typename Clock>
      struct is_clock : bool_constant<is_clock_v<Clock>>
      {
      };

#endif
    } // namespace chrono
  }   // namespace v1
} // namespace rsl
