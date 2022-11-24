#pragma once

namespace rsl
{
  inline namespace v1
  {
    template <typename Pair>
    struct use_second
    {
      using argument_type = Pair;
      using result_type   = typename Pair::second_type;

      const result_type& operator()(const Pair& x) const
      {
        return x.second;
      }
    };
  } // namespace v1
} // namespace rsl