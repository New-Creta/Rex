

#pragma once

//
// Custom sink for mongodb
// Building and using requires mongocxx library.
// For building mongocxx library check the url below
// http://mongocxx.org/mongocxx-v3/installation/
//

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/types.hpp>
#include <bsoncxx/view_or_value.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/uri.hpp>
#include <rex_engine/diagnostics/logging/internal/details/synchronous_factory.h>

namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class mongo_sink : public base_sink<Mutex>
    {
    public:
      mongo_sink(const rsl::string& db_name, const rsl::string& collection_name, const rsl::string& uri = "mongodb://localhost:27017")
      try : mongo_sink(rsl::make_shared<mongocxx::instance>(), db_name, collection_name, uri)
      {
      }
      catch(const rsl::exception& e)
      {
        throw_rexlog_ex(fmt_lib::format("Error opening database: {}", e.what()));
      }

      mongo_sink(rsl::shared_ptr<mongocxx::instance> instance, const rsl::string& db_name, const rsl::string& collection_name, const rsl::string& uri = "mongodb://localhost:27017")
          : instance_(rsl::move(instance))
          , db_name_(db_name)
          , coll_name_(collection_name)
      {
        try
        {
          client_ = rexlog::details::make_unique<mongocxx::client>(mongocxx::uri {uri});
        }
        catch(const rsl::exception& e)
        {
          throw_rexlog_ex(fmt_lib::format("Error opening database: {}", e.what()));
        }
      }

      ~mongo_sink()
      {
        flush_();
      }

    protected:
      void sink_it_(const details::log_msg& msg) override
      {
        using bsoncxx::builder::stream::document;
        using bsoncxx::builder::stream::finalize;

        if(client_ != nullptr)
        {
          auto doc = document {} << "timestamp" << bsoncxx::types::b_date(msg.time) << "level" << level::to_string_view(msg.level).data() << "level_num" << msg.level << "message" << rsl::string(msg.payload.begin(), msg.payload.end()) << "logger_name"
                                 << rsl::string(msg.logger_name.begin(), msg.logger_name.end()) << "thread_id" << static_cast<int>(msg.thread_id) << finalize;
          client_->database(db_name_).collection(coll_name_).insert_one(doc.view());
        }
      }

      void flush_() override {}

    private:
      rsl::shared_ptr<mongocxx::instance> instance_;
      rsl::string db_name_;
      rsl::string coll_name_;
      rsl::unique_ptr<mongocxx::client> client_ = nullptr;
    };

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

#include <mutex>
    using mongo_sink_mt = mongo_sink<rsl::mutex>;
    using mongo_sink_st = mongo_sink<rexlog::details::null_mutex>;

  } // namespace sinks

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> mongo_logger_mt(const rsl::string& logger_name, const rsl::string& db_name, const rsl::string& collection_name, const rsl::string& uri = "mongodb://localhost:27017")
  {
    return Factory::template create<sinks::mongo_sink_mt>(logger_name, db_name, collection_name, uri);
  }

  template <typename Factory = rexlog::synchronous_factory>
  inline rsl::shared_ptr<logger> mongo_logger_st(const rsl::string& logger_name, const rsl::string& db_name, const rsl::string& collection_name, const rsl::string& uri = "mongodb://localhost:27017")
  {
    return Factory::template create<sinks::mongo_sink_st>(logger_name, db_name, collection_name, uri);
  }

} // namespace rexlog