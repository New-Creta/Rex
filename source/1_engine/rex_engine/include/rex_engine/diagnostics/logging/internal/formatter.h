

#pragma once

#include <format>
#include <rexlog/details/log_msg.h>

namespace rexlog {

class formatter
{
public:
    virtual ~formatter() = default;
    virtual void format(const details::log_msg &msg, memory_buf_t &dest) = 0;
    virtual rsl::unique_ptr<formatter> clone() const = 0;
};
} // namespace rexlog
