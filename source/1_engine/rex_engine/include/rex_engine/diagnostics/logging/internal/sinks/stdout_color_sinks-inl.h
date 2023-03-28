

#pragma once

#include <rexlog/sinks/stdout_color_sinks.h>

#include <rexlog/logger.h>
#include <rexlog/common.h>

namespace rexlog {

template<typename Factory>
REXLOG_INLINE rsl::shared_ptr<logger> stdout_color_mt(const rsl::string &logger_name, color_mode mode)
{
    return Factory::template create<sinks::stdout_color_sink_mt>(rsl::string(logger_name), mode);
}

template<typename Factory>
REXLOG_INLINE rsl::shared_ptr<logger> stdout_color_st(const rsl::string &logger_name, color_mode mode)
{
    return Factory::template create<sinks::stdout_color_sink_st>(rsl::string(logger_name), mode);
}

template<typename Factory>
REXLOG_INLINE rsl::shared_ptr<logger> stderr_color_mt(const rsl::string &logger_name, color_mode mode)
{
    return Factory::template create<sinks::stderr_color_sink_mt>(rsl::string(logger_name), mode);
}

template<typename Factory>
REXLOG_INLINE rsl::shared_ptr<logger> stderr_color_st(const rsl::string &logger_name, color_mode mode)
{
    return Factory::template create<sinks::stderr_color_sink_st>(rsl::string(logger_name), mode);
}
} // namespace rexlog
