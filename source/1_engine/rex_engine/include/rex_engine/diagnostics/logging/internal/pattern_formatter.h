

#pragma once

#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

#include <chrono>
#include <ctime>
#include <rex_engine/diagnostics/logging/internal/common.h>
#include <rex_engine/diagnostics/logging/internal/details/log_msg.h>
#include <rex_engine/diagnostics/logging/internal/details/os.h>
#include <rex_engine/diagnostics/logging/internal/formatter.h>
#include <string>

namespace rexlog
{
  namespace details
  {

    // padding information.
    struct padding_info
    {
      enum class pad_side
      {
        left,
        right,
        center
      };

      padding_info() = default;
      padding_info(size_t width, padding_info::pad_side side, bool truncate)
          : width_(width)
          , side_(side)
          , truncate_(truncate)
          , enabled_(true)
      {
      }

      bool enabled() const
      {
        return enabled_;
      }
      size_t width_  = 0;
      pad_side side_ = pad_side::left;
      bool truncate_ = false;
      bool enabled_  = false;
    };

    class REXLOG_API flag_formatter
    {
    public:
      explicit flag_formatter(padding_info padinfo)
          : padinfo_(padinfo)
      {
      }
      flag_formatter()                                                                        = default;
      virtual ~flag_formatter()                                                               = default;
      virtual void format(const details::log_msg& msg, const tm& tm_time, memory_buf_t& dest) = 0;

    protected:
      padding_info padinfo_;
    };

  } // namespace details

  class REXLOG_API custom_flag_formatter : public details::flag_formatter
  {
  public:
    virtual rsl::unique_ptr<custom_flag_formatter> clone() const = 0;

    void set_padding_info(const details::padding_info& padding)
    {
      flag_formatter::padinfo_ = padding;
    }
  };

  class REXLOG_API pattern_formatter final : public formatter
  {
  public:
    using custom_flags = rsl::unordered_map<char, rsl::unique_ptr<custom_flag_formatter>>;

    explicit pattern_formatter(rsl::string pattern, pattern_time_type time_type = pattern_time_type::local, rsl::string eol = rsl::string(rexlog::details::os::default_eol), custom_flags custom_user_flags = custom_flags());

    // use default pattern is not given
    explicit pattern_formatter(pattern_time_type time_type = pattern_time_type::local, rsl::string eol = rsl::string(rexlog::details::os::default_eol));

    pattern_formatter(const pattern_formatter& other)            = delete;
    pattern_formatter& operator=(const pattern_formatter& other) = delete;

    rsl::unique_ptr<formatter> clone() const override;
    void format(const details::log_msg& msg, memory_buf_t& dest) override;

    template <typename T, typename... Args>
    pattern_formatter& add_flag(char flag, Args&&... args)
    {
      custom_handlers_[flag] = details::make_unique<T>(rsl::forward<Args>(args)...);
      return *this;
    }
    void set_pattern(rsl::string pattern);
    void need_localtime(bool need = true);

  private:
    rsl::string pattern_;
    rsl::string eol_;
    pattern_time_type pattern_time_type_;
    bool need_localtime_;
    tm cached_tm_;
    rsl::chrono::seconds last_log_secs_;
    rsl::vector<rsl::unique_ptr<details::flag_formatter>> formatters_;
    custom_flags custom_handlers_;

    tm get_time_(const details::log_msg& msg);
    template <typename Padder>
    void handle_flag_(char flag, details::padding_info padding);

    // Extract given pad spec (e.g. %8X)
    // Advance the given it pass the end of the padding spec found (if any)
    // Return padding.
    static details::padding_info handle_padspec_(rsl::string::const_iterator& it, rsl::string::const_iterator end);

    void compile_pattern_(const rsl::string& pattern);
  };
} // namespace rexlog