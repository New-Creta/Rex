
#pragma once

//
// Custom sink for QPlainTextEdit or QTextEdit and its childs(QTextBrowser...
// etc) Building and using requires Qt library.
//

#include "rex_engine/diagnostics/logging/internal/common.h"
#include "rex_engine/diagnostics/logging/internal/details/log_msg.h"
#include "rex_engine/diagnostics/logging/internal/details/synchronous_factory.h"
#include "rex_engine/diagnostics/logging/internal/sinks/base_sink.h"

#include <QPlainTextEdit>
#include <QTextEdit>

//
// qt_sink class
//
namespace rexlog
{
  namespace sinks
  {
    template <typename Mutex>
    class qt_sink : public BaseSink<Mutex>
    {
    public:
      qt_sink(QObject* qt_object, const rex::DebugString& meta_method)
      {
        qt_object_   = qt_object;
        meta_method_ = meta_method;
      }

      ~qt_sink()
      {
        flush_impl();
      }

    protected:
      void sink_it_impl(const details::LogMsg& msg) override
      {
        memory_buf_t formatted;
        BaseSink<Mutex>::m_formatter->format(msg, formatted);
        string_view_t str = string_view_t(formatted.data(), formatted.size());
        QMetaObject::invokeMethod(qt_object_, meta_method_.c_str(), Qt::AutoConnection, Q_ARG(QString, QString::fromUtf8(str.data(), static_cast<int>(str.size())).trimmed()));
      }

      void flush_impl() override {}

    private:
      QObject* qt_object_ = nullptr;
      rex::DebugString meta_method_;
    };

#include "rex_engine/diagnostics/logging/internal/details/null_mutex.h"

#include <mutex>
    using qt_sink_mt = qt_sink<rsl::mutex>;
    using qt_sink_st = qt_sink<rexlog::details::NullMutex>;
  } // namespace sinks

  //
  // Factory functions
  //
  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_mt(const rex::DebugString& logger_name, QTextEdit* qt_object, const rex::DebugString& meta_method = "append")
  {
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_st(const rex::DebugString& logger_name, QTextEdit* qt_object, const rex::DebugString& meta_method = "append")
  {
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_mt(const rex::DebugString& logger_name, QPlainTextEdit* qt_object, const rex::DebugString& meta_method = "appendPlainText")
  {
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_st(const rex::DebugString& logger_name, QPlainTextEdit* qt_object, const rex::DebugString& meta_method = "appendPlainText")
  {
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_mt(const rex::DebugString& logger_name, QObject* qt_object, const rex::DebugString& meta_method)
  {
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
  }

  template <typename Factory = rexlog::SynchronousFactory>
  inline rsl::shared_ptr<Logger> qt_logger_st(const rex::DebugString& logger_name, QObject* qt_object, const rex::DebugString& meta_method)
  {
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
  }
} // namespace rexlog
