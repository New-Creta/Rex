

#pragma once

#include "registry.h"

namespace rexlog
{

  // Default logger factory-  creates synchronous loggers
  class logger;

  struct SynchronousFactory
  {
    template <typename Sink, typename... SinkArgs>
    static rsl::shared_ptr<rexlog::logger> create(rex::DebugString logger_name, SinkArgs&&... args)
    {
      auto sink       = rsl::allocate_shared<Sink>(rex::global_debug_allocator(), rsl::forward<SinkArgs>(args)...);
      auto new_logger = rsl::allocate_shared<rexlog::logger>(rex::global_debug_allocator(), rsl::move(logger_name), rsl::move(sink));
      details::Registry::instance().initialize_logger(new_logger);
      return new_logger;
    }
  };
} // namespace rexlog
