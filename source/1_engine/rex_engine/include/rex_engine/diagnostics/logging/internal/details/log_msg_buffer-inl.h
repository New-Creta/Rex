

#pragma once

#include <rex_engine/diagnostics/logging/internal/details/log_msg_buffer.h>

namespace rexlog {
namespace details {

REXLOG_INLINE log_msg_buffer::log_msg_buffer(const log_msg &orig_msg)
    : log_msg{orig_msg}
{
    buffer.append(logger_name.begin(), logger_name.end());
    buffer.append(payload.begin(), payload.end());
    update_string_views();
}

REXLOG_INLINE log_msg_buffer::log_msg_buffer(const log_msg_buffer &other)
    : log_msg{other}
{
    buffer.append(logger_name.begin(), logger_name.end());
    buffer.append(payload.begin(), payload.end());
    update_string_views();
}

REXLOG_INLINE log_msg_buffer::log_msg_buffer(log_msg_buffer &&other) REXLOG_NOEXCEPT : log_msg{other}, buffer{rsl::move(other.buffer)}
{
    update_string_views();
}

REXLOG_INLINE log_msg_buffer &log_msg_buffer::operator=(const log_msg_buffer &other)
{
    log_msg::operator=(other);
    buffer.clear();
    buffer.append(other.buffer.data(), other.buffer.data() + other.buffer.size());
    update_string_views();
    return *this;
}

REXLOG_INLINE log_msg_buffer &log_msg_buffer::operator=(log_msg_buffer &&other) REXLOG_NOEXCEPT
{
    log_msg::operator=(other);
    buffer = rsl::move(other.buffer);
    update_string_views();
    return *this;
}

REXLOG_INLINE void log_msg_buffer::update_string_views()
{
    logger_name = string_view_t{buffer.data(), logger_name.size()};
    payload = string_view_t{buffer.data() + logger_name.size(), payload.size()};
}

} // namespace details
} // namespace rexlog
