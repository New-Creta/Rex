

#pragma once

#include <chrono>
#include <format>

// Stopwatch support for rexlog  (using rsl::chrono::steady_clock).
// Displays elapsed seconds since construction as double.
//
// Usage:
//
// rexlog::stopwatch sw;
// ...
// rexlog::debug("Elapsed: {} seconds", sw);    =>  "Elapsed 0.005116733 seconds"
// rexlog::info("Elapsed: {:.6} seconds", sw);  =>  "Elapsed 0.005163 seconds"
//
//
// If other units are needed (e.g. millis instead of double), include "<chrono>" and use "duration_cast<..>(sw.elapsed())":
//
// #include <chrono>
//
// using rsl::chrono::duration_cast;
// using rsl::chrono::milliseconds;
// rexlog::info("Elapsed {}", duration_cast<milliseconds>(sw.elapsed())); => "Elapsed 5ms"

namespace rexlog
{
  class stopwatch
  {
    using clock = rsl::chrono::steady_clock;
    rsl::chrono::time_point<clock> start_tp_;

  public:
    stopwatch()
        : start_tp_ {clock::now()}
    {
    }

    rsl::chrono::duration<double> elapsed() const
    {
      return rsl::chrono::duration<double>(clock::now() - start_tp_);
    }

    void reset()
    {
      start_tp_ = clock::now();
    }
  };
} // namespace rexlog

// Support for fmt formatting  (e.g. "{:012.9}" or just "{}")
namespace rsl
{

  template <>
  struct formatter<rexlog::stopwatch> : formatter<double>
  {
    template <typename FormatContext>
    auto format(const rexlog::stopwatch& sw, FormatContext& ctx) -> decltype(ctx.out())
    {
      return formatter<double>::format(sw.elapsed().count(), ctx);
    }
  };
} // namespace rsl
