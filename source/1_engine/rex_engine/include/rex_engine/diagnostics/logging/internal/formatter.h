

#pragma once

#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"

namespace rexlog
{

  class formatter
  {
  public:
    virtual ~formatter()                                                = default;
    virtual void format(const details::LogMsg& msg, memory_buf_t& dest) = 0;
    virtual rsl::unique_ptr<formatter> clone() const                    = 0;
  };
} // namespace rexlog