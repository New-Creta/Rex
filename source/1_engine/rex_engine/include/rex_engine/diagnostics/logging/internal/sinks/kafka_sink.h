

#pragma once

//
// Custom sink for kafka
// Building and using requires librdkafka library.
// For building librdkafka library check the url below
// https://github.com/confluentinc/librdkafka
//

#include "rex_engine/diagnostics/logging/internal/async.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"
#include "rex_engine/diagnostics/logging/internal/details/SynchronousFactory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <mutex>
#include "rex_engine/diagnostics/logging/internal/common.h"

// kafka header
#include <librdkafka/rdkafkacpp.h"

namespace rexlog
{
  namespace sinks
  {

    struct kafka_sink_config
    {
      rex::DebugString server_addr;
      rex::DebugString produce_topic;
      int32_t flush_timeout_ms = 1000;

      kafka_sink_config(rex::DebugString addr, rex::DebugString topic, int flush_timeout_ms = 1000)
          : server_addr {rsl::move(addr)}
          , produce_topic {rsl::move(topic)}
          , flush_timeout_ms(flush_timeout_ms)
      {
      }
    };

    template <typename Mutex>
    class kafka_sink : public BaseSink<Mutex>
    {
    public:
      kafka_sink(kafka_sink_config config)
          : config_ {rsl::move(config)}
      {
        try
        {
          rex::DebugString errstr;
          conf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_GLOBAL));
          RdKafka::Conf::ConfResult confRes = conf_->set("bootstrap.servers", config_.server_addr, errstr);
          if(confRes != RdKafka::Conf::CONF_OK)
          {
            throw_rexlog_ex(fmt_lib::format("conf set bootstrap.servers failed err:{}", errstr));
          }

          tconf_.reset(RdKafka::Conf::create(RdKafka::Conf::CONF_TOPIC));
          if(tconf_ == nullptr)
          {
            throw_rexlog_ex(fmt_lib::format("create topic config failed"));
          }

          producer_.reset(RdKafka::Producer::create(conf_.get(), errstr));
          if(producer_ == nullptr)
          {
            throw_rexlog_ex(fmt_lib::format("create producer failed err:{}", errstr));
          }
          topic_.reset(RdKafka::Topic::create(producer_.get(), config_.produce_topic, tconf_.get(), errstr));
          if(topic_ == nullptr)
          {
            throw_rexlog_ex(fmt_lib::format("create topic failed err:{}", errstr));
          }
        }
        catch(const rsl::exception& e)
        {
          throw_rexlog_ex(fmt_lib::format("error create kafka instance: {}", e.what()));
        }
      }

      ~kafka_sink()
      {
        producer_->flush(config_.flush_timeout_ms);
      }

    protected:
      void sink_it_(const details::LogMsg& msg) override
      {
        producer_->produce(topic_.get(), 0, RdKafka::Producer::RK_MSG_COPY, (void*)msg.payload.data(), msg.payload.size(), NULL, NULL);
      }

      void flush_() override
      {
        producer_->flush(config_.flush_timeout_ms);
      }

    private:
      kafka_sink_config config_;
      rsl::unique_ptr<RdKafka::Producer> producer_ = nullptr;
      rsl::unique_ptr<RdKafka::Conf> conf_         = nullptr;
      rsl::unique_ptr<RdKafka::Conf> tconf_        = nullptr;
      rsl::unique_ptr<RdKafka::Topic> topic_       = nullptr;
    };

    using kafka_sink_mt = kafka_sink<rsl::mutex>;
    using kafka_sink_st = kafka_sink<rexlog::details::NullMutex>;

  } // namespace sinks

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<logger> kafka_logger_mt(const rex::DebugString& logger_name, rexlog::sinks::kafka_sink_config config)
  {
    return Factory::template create<sinks::kafka_sink_mt>(logger_name, config);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<logger> kafka_logger_st(const rex::DebugString& logger_name, rexlog::sinks::kafka_sink_config config)
  {
    return Factory::template create<sinks::kafka_sink_st>(logger_name, config);
  }

  template <typename Factory = rexlog::async_factory>
  inline rsl::shared_ptr<rexlog::logger> kafka_logger_async_mt(rex::DebugString logger_name, rexlog::sinks::kafka_sink_config config)
  {
    return Factory::template create<sinks::kafka_sink_mt>(logger_name, config);
  }

  template <typename Factory = rexlog::async_factory>
  inline rsl::shared_ptr<rexlog::logger> kafka_logger_async_st(rex::DebugString logger_name, rexlog::sinks::kafka_sink_config config)
  {
    return Factory::template create<sinks::kafka_sink_st>(logger_name, config);
  }

} // namespace rexlog
