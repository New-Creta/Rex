

#pragma once

#include "registry.h"

namespace rexlog {

// Default logger factory-  creates synchronous loggers
class logger;

struct synchronous_factory
{
    template<typename Sink, typename... SinkArgs>
    static rsl::shared_ptr<rexlog::logger> create(rsl::string logger_name, SinkArgs &&... args)
    {
        auto sink = rsl::make_shared<Sink>(rsl::forward<SinkArgs>(args)...);
        auto new_logger = rsl::make_shared<rexlog::logger>(rsl::move(logger_name), rsl::move(sink));
        details::registry::instance().initialize_logger(new_logger);
        return new_logger;
    }
};
} // namespace rexlog
