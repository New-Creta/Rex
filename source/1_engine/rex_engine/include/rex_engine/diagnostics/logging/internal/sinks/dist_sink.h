

#pragma once

#include "base_sink.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/string/stack_string.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

// Distribution sink (mux). Stores a vector of sinks which get called when log
// is called

namespace rex
{
  namespace log
  {
    namespace sinks
    {
      template <typename Mutex>
      class DistSink : public BaseSink<Mutex>
      {
      public:
        DistSink() = default;
        explicit DistSink(const rex::DebugVector<rsl::shared_ptr<AbstractSink>>& sinks);

        DistSink(const DistSink&)            = delete;
        DistSink& operator=(const DistSink&) = delete;

        ~DistSink() override = default;

        void add_sink(rsl::shared_ptr<AbstractSink> subSink);
        void remove_sink(rsl::shared_ptr<AbstractSink> subSink);
        void set_sinks(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks);

      protected:
        void sink_it_impl(const details::LogMsg& msg) override;
        void flush_it_impl() override;
        void set_pattern_impl(rsl::string_view pattern) override;
        void set_formatter_impl(PatternFormatter sinkFormatter) override;

      private:
        rex::DebugVector<rsl::shared_ptr<AbstractSink>> m_sinks;
      };

      template <typename Mutex>
      DistSink<Mutex>::DistSink(const rex::DebugVector<rsl::shared_ptr<AbstractSink>>& sinks)
          : m_sinks(sinks)
      {
      }

      template <typename Mutex>
      void DistSink<Mutex>::add_sink(rsl::shared_ptr<AbstractSink> subSink)
      {
        rsl::unique_lock<Mutex> lock(BaseSink<Mutex>::m_mutex);
        m_sinks.push_back(rsl::move(subSink));
      }

      template <typename Mutex>
      void DistSink<Mutex>::remove_sink(rsl::shared_ptr<AbstractSink> subSink)
      {
        rsl::unique_lock<Mutex> lock(BaseSink<Mutex>::m_mutex);
        m_sinks.erase(rsl::remove(m_sinks.begin(), m_sinks.end(), subSink), m_sinks.end());
      }

      template <typename Mutex>
      void DistSink<Mutex>::set_sinks(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks)
      {
        rsl::unique_lock<Mutex> lock(BaseSink<Mutex>::m_mutex);
        m_sinks = rsl::move(sinks);
      }

      template <typename Mutex>
      void DistSink<Mutex>::sink_it_impl(const details::LogMsg& msg)
      {
        for(auto& sub_sink: m_sinks)
        {
          if(sub_sink->should_log(msg.level()))
          {
            sub_sink->log(msg);
          }
        }
      }

      template <typename Mutex>
      void DistSink<Mutex>::flush_it_impl()
      {
        for(auto& sub_sink: m_sinks)
        {
          sub_sink->flush();
        }
      }

      template <typename Mutex>
      void DistSink<Mutex>::set_pattern_impl(rsl::string_view pattern)
      {
        set_formatter_impl(PatternFormatter(pattern));
      }

      template <typename Mutex>
      void DistSink<Mutex>::set_formatter_impl(PatternFormatter sinkFormatter)
      {
        BaseSink<Mutex>::set_formatter_impl(rsl::move(sinkFormatter));

        for(auto& sub_sink: m_sinks)
        {
          sub_sink->set_formatter(BaseSink<Mutex>::formatter().clone());
        }
      }

      using DistSink_mt = DistSink<rsl::mutex>;
      using DistSink_st = DistSink<details::NullMutex>;

    } // namespace sinks
  }   // namespace log
} // namespace rex
