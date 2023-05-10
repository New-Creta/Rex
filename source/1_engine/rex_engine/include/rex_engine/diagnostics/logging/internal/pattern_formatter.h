

#pragma once

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/os.h"
#include "rex_engine/diagnostics/logging/internal/formatter.h"
#include "rex_engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"
#include "rex_std/vector.h"

namespace rexlog
{
  namespace details
  {

    // padding information.
    struct PaddingInfo
    {
      enum class PadSide
      {
        Left,
        Right,
        Center
      };

      PaddingInfo() = default;
      PaddingInfo(size_t width, PaddingInfo::PadSide side, bool truncate)
          : width(width)
          , side(side)
          , truncate(truncate)
          , m_enabled(true)
      {
      }

      bool enabled() const
      {
        return m_enabled;
      }
      size_t width   = 0;
      PadSide side   = PadSide::Left;
      bool truncate  = false;
      bool m_enabled = false;
    };

    class REXLOG_API FlagFormatter
    {
    public:
      explicit FlagFormatter(PaddingInfo padinfo)
          : m_padinfo(padinfo)
      {
      }
      FlagFormatter()                                                                       = default;
      virtual ~FlagFormatter()                                                              = default;
      virtual void format(const details::LogMsg& msg, const tm& tmTime, memory_buf_t& dest) = 0;

    protected:
      void set_padding_info(const details::PaddingInfo& padding)
      {
        m_padinfo = padding;
      }

      PaddingInfo& paddinginfo()
      {
        return m_padinfo;
      }

    private:
      PaddingInfo m_padinfo;
    };

  } // namespace details

  class REXLOG_API CustomFlagFormatter : public details::FlagFormatter
  {
  public:
    virtual rsl::unique_ptr<CustomFlagFormatter> clone() const = 0;

    void set_padding_info(const details::PaddingInfo& padding)
    {
      FlagFormatter::set_padding_info(padding);
    }
  };

  class REXLOG_API PatternFormatter final : public formatter
  {
  public:
    using custom_flags = rex::DebugHashTable<char, rsl::unique_ptr<CustomFlagFormatter>>;

    explicit PatternFormatter(rsl::string_view pattern, PatternTimeType timeType = PatternTimeType::Local, rex::DebugString eol = rex::DebugString(rexlog::details::os::default_eol), custom_flags customUserFlags = custom_flags());

    // use default pattern is not given
    explicit PatternFormatter(PatternTimeType timeType = PatternTimeType::Local, rex::DebugString eol = rex::DebugString(rexlog::details::os::default_eol));

    PatternFormatter(const PatternFormatter& other) = delete;
    PatternFormatter(PatternFormatter&& other)      = delete;
    ~PatternFormatter() override                    = default;

    PatternFormatter& operator=(const PatternFormatter& other) = delete;
    PatternFormatter& operator=(PatternFormatter&& other)      = delete;

    rsl::unique_ptr<formatter> clone() const override;
    void format(const details::LogMsg& msg, memory_buf_t& dest) override;

    template <typename T, typename... Args>
    PatternFormatter& add_flag(char flag, Args&&... args)
    {
      custom_handlers_[flag] = details::make_unique<T>(rsl::forward<Args>(args)...);
      return *this;
    }
    void set_pattern(rsl::string_view pattern);
    void need_localtime(bool need = true);

  private:
    rex::DebugString m_pattern;
    rex::DebugString m_eol;
    PatternTimeType m_pattern_time_type;
    bool m_need_localtime;
    tm m_cached_tm {};
    rsl::chrono::seconds m_last_log_secs;
    rex::DebugVector<rsl::unique_ptr<details::FlagFormatter>> m_formatters;
    custom_flags m_custom_handlers;

    tm get_time_impl(const details::LogMsg& msg);
    template <typename Padder>
    void handle_flag_impl(char flag, details::PaddingInfo padding);

    // Extract given pad spec (e.g. %8X)
    // Advance the given it pass the end of the padding spec found (if any)
    // Return padding.
    static details::PaddingInfo handle_padspec_impl(rex::DebugString::const_iterator& it, rex::DebugString::const_iterator end);

    void compile_pattern_impl(rsl::string_view pattern);
  };
} // namespace rexlog