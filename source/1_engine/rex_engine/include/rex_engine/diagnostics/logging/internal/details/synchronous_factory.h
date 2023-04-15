

#pragma once

#include "registry.h"

namespace rexlog
{

  // Default Logger factory-  creates synchronous loggers
  class Logger;

  struct SynchronousFactory
  {
    template <typename Sink, typename... SinkArgs>
    static rsl::shared_ptr<rexlog::Logger> create(rex::DebugString loggerName, SinkArgs&&... args)
    {
      auto sink       = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
      auto new_logger = rsl::allocate_shared<rexlog::Logger>(rex::global_debug_allocator(), rsl::move(loggerName), rsl::move(sink));
      details::Registry::instance().initialize_logger(new_logger);
      return new_logger;
    }
  };
} // namespace rexlog
