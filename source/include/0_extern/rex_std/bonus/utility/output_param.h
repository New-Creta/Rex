// ============================================
//
// REX - STANDARD LIBRARY IMPLEMENTATION
//
// Author: Nick De Breuck
// Twitter: @nick_debreuck
//
// File: output_param.h
// Copyright (c) Nick De Breuck 2022
//
// ============================================

#pragma once

namespace rsl
{
  inline namespace v1
  {

    template <typename T>
    class Out
    {
    public:
      using value_type = T;

      explicit Out(value_type& output)
          : m_output(output)
      {
      }

      value_type& get()
      {
        return m_output;
      }

      Out& operator=(const value_type& rhs)
      {
        m_output = rhs;
      }

    private:
      value_type& m_output;
    };

  } // namespace v1
} // namespace rsl