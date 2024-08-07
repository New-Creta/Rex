
#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_std/bonus/time/time_digits.h"
#include "rex_std/string.h"

// Some fmt helpers to efficiently format and pad ints and strings
namespace rex
{
  namespace log
  {
    namespace details
    {
      namespace fmt_helper
      {

        inline void append_string_view(rsl::string_view view, memory_buf_t& dest)
        {
          dest += view;
        }

        template <typename T>
        inline void append_int(T n, memory_buf_t& dest)
        {
          static_assert(rsl::is_integral_v<T>, "n needs to be an integral value");

          auto x = rsl::to_string(n);

          dest.append(x.data(), x.size());
        }

        template <typename T>
        constexpr unsigned int count_digits_fallback(T n)
        {
          // taken from fmt: https://github.com/fmtlib/fmt/blob/8.0.1/include/fmt/format.h#L899-L912
          unsigned int count = 1;
          for(;;)
          {
            // Integer division is slow so do it for a group of four digits instead
            // of for every digit. The idea comes from the talk by Alexandrescu
            // "Three Optimization Tips for C++". See speed-test for a comparison.
            if(n < 10)
              return count;
            if(n < 100)
              return count + 1;
            if(n < 1000)
              return count + 2;
            if(n < 10000)
              return count + 3;
            n /= 10000u;
            count += 4;
          }
        }

        template <typename T>
        inline unsigned int count_digits(T n)
        {
          using count_type = typename rsl::conditional<(sizeof(T) > sizeof(uint32_t)), uint64_t, uint32_t>::type;
          return count_digits_fallback(static_cast<count_type>(n));
        }

        inline void pad2(int n, memory_buf_t& dest)
        {
          if(n >= 0 && n < 100) // 0-99
          {
            const rsl::time_digits time_digits(n);
            dest += time_digits.to_string();
          }
          else // unlikely, but just in case, let fmt deal with it
          {
            rsl::format_to(rsl::back_inserter(dest), "{:02}", n);
          }
        }

        template <typename T>
        inline void pad_uint(T n, unsigned int width, memory_buf_t& dest)
        {
          static_assert(rsl::is_unsigned<T>::value, "pad_uint must get unsigned T");
          for(auto digits = count_digits(n); digits < width; digits++)
          {
            dest.push_back('0');
          }
          append_int(n, dest);
        }

        template <typename T>
        inline void pad3(T n, memory_buf_t& dest)
        {
          static_assert(rsl::is_unsigned<T>::value, "pad3 must get unsigned T");
          if(n < 1000)
          {
            dest.push_back(static_cast<char>(n / 100 + '0'));
            n = n % 100;
            const rsl::time_digits time_digits(n);
            dest += time_digits.to_string();
          }
          else
          {
            append_int(n, dest);
          }
        }

        template <typename T>
        inline void pad6(T n, memory_buf_t& dest)
        {
          pad_uint(n, 6, dest);
        }

        template <typename T>
        inline void pad9(T n, memory_buf_t& dest)
        {
          pad_uint(n, 9, dest);
        }

        // return fraction of a second of the given time_point.
        // e.g.
        // fraction<rsl::milliseconds>(tp) -> will return the millis part of the second
        template <typename ToDuration>
        inline ToDuration time_fraction(log_clock::time_point tp)
        {
          using rsl::chrono::duration_cast;
          using rsl::chrono::seconds;
          auto duration = tp.time_since_epoch();
          auto secs     = duration_cast<seconds>(duration);
          return duration_cast<ToDuration>(duration) - duration_cast<ToDuration>(secs);
        }

      } // namespace fmt_helper
    }   // namespace details
  }     // namespace log
} // namespace rex
