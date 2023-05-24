

#pragma once

#include "base_sink.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/pattern_formatter.h"
#include "rex_engine/types.h"
#include "rex_std/memory.h"
#include "rex_std/vector.h"

#include <algorithm>
#include <mutex>

// Distribution sink (mux). Stores a vector of sinks which get called when log
// is called

namespace rexlog
{
    namespace sinks
    {
        template <typename Mutex>
        class dist_sink : public BaseSink<Mutex>
        {
        public:
            dist_sink() = default;
            explicit dist_sink(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks);

            dist_sink(const dist_sink&) = delete;
            dist_sink& operator=(const dist_sink&) = delete;

            void add_sink(rsl::shared_ptr<AbstractSink> sub_sink);
            void remove_sink(rsl::shared_ptr<AbstractSink> sub_sink);
            void set_sinks(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks);
            rex::DebugVector<rsl::shared_ptr<AbstractSink>>& sinks();

        protected:
            void sink_it_impl(const details::LogMsg& msg) override;
            void flush_impl() override;
            void set_pattern_impl(const rex::DebugString& pattern) override;
            void set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sink_formatter) override;

        private:
            rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks_;
        };

        template <typename Mutex>
        dist_sink<Mutex>::dist_sink(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks)
            : sinks_impl(sinks)
        {
        }

        template <typename Mutex>
        void dist_sink<Mutex>::add_sink(rsl::shared_ptr<AbstractSink> sub_sink)
        {
            rsl::lock_guard<Mutex> lock(BaseSink<Mutex>::m_mutex);
            sinks_.push_back(sub_sink);
        }

        template <typename Mutex>
        void dist_sink<Mutex>::remove_sink(rsl::shared_ptr<AbstractSink> sub_sink)
        {
            rsl::lock_guard<Mutex> lock(BaseSink<Mutex>::m_mutex);
            sinks_.erase(rsl::remove(sinks_.begin(), sinks_.end(), sub_sink), sinks_.end());
        }

        template <typename Mutex>
        void dist_sink<Mutex>::set_sinks(rex::DebugVector<rsl::shared_ptr<AbstractSink>> sinks)
        {
            rsl::lock_guard<Mutex> lock(BaseSink<Mutex>::m_mutex);
            sinks_ = rsl::move(sinks);
        }

        template <typename Mutex>
        rex::DebugVector<rsl::shared_ptr<AbstractSink>>& dist_sink<Mutex>::sinks()
        {
            return sinks_;
        }

        template <typename Mutex>
        void dist_sink<Mutex>::sink_it_impl(const details::LogMsg& msg)
        {
            for (auto& sub_sink : sinks_)
            {
                if (sub_sink->should_log(msg.level))
                {
                    sub_sink->log(msg);
                }
            }
        }

        template <typename Mutex>
        void dist_sink<Mutex>::flush_impl()
        {
            for (auto& sub_sink : sinks_)
            {
                sub_sink->flush();
            }
        }

        template <typename Mutex>
        void dist_sink<Mutex>::set_pattern_impl(const rex::DebugString& pattern)
        {
            set_formatter_impl(details::make_unique<rexlog::pattern_formatter>(pattern));
        }

        template <typename Mutex>
        void dist_sink<Mutex>::set_formatter_impl(rsl::unique_ptr<rexlog::formatter> sink_formatter)
        {
            BaseSink<Mutex>::m_formatter = rsl::move(sink_formatter);
            for (auto& sub_sink : sinks_)
            {
                sub_sink->set_formatter(BaseSink<Mutex>::m_formatter->clone());
            }
        }

        using dist_sink_mt = dist_sink<rsl::mutex>;
        using dist_sink_st = dist_sink<details::NullMutex>;

    } // namespace sinks
} // namespace rexlog
