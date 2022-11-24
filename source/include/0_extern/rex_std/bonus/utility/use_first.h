#pragma once

namespace rsl
{
  inline namespace v1
  {
    template <typename Pair>
    struct use_first
    {
      using argument_type = Pair;
      using result_type   = typename Pair::first_type;

      const result_type& operator()(const Pair& x) const
      {
        return x.first();
      }
    };
  } // namespace v1
} // namespace rsl