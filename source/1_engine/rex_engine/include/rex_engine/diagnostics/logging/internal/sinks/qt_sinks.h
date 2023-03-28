
#pragma once

//
// Custom sink for QPlainTextEdit or QTextEdit and its childs(QTextBrowser...
// etc) Building and using requires Qt library.
//

#include "rexlog/common.h"
#include "rexlog/details/log_msg.h"
#include "rexlog/details/synchronous_factory.h"
#include "rexlog/sinks/base_sink.h"

#include <QTextEdit>
#include <QPlainTextEdit>

//
// qt_sink class
//
namespace rexlog {
namespace sinks {
template<typename Mutex>
class qt_sink : public base_sink<Mutex>
{
public:
    qt_sink(QObject *qt_object, const rsl::string &meta_method)
    {
        qt_object_ = qt_object;
        meta_method_ = meta_method;
    }

    ~qt_sink()
    {
        flush_();
    }

protected:
    void sink_it_(const details::log_msg &msg) override
    {
        memory_buf_t formatted;
        base_sink<Mutex>::formatter_->format(msg, formatted);
        string_view_t str = string_view_t(formatted.data(), formatted.size());
        QMetaObject::invokeMethod(qt_object_, meta_method_.c_str(), Qt::AutoConnection,
            Q_ARG(QString, QString::fromUtf8(str.data(), static_cast<int>(str.size())).trimmed()));
    }

    void flush_() override {}

private:
    QObject *qt_object_ = nullptr;
    rsl::string meta_method_;
};

#include "rexlog/details/null_mutex.h"
#include <mutex>
using qt_sink_mt = qt_sink<rsl::mutex>;
using qt_sink_st = qt_sink<rexlog::details::null_mutex>;
} // namespace sinks

//
// Factory functions
//
template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_mt(const rsl::string &logger_name, QTextEdit *qt_object, const rsl::string &meta_method = "append")
{
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
}

template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_st(const rsl::string &logger_name, QTextEdit *qt_object, const rsl::string &meta_method = "append")
{
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
}

template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_mt(
    const rsl::string &logger_name, QPlainTextEdit *qt_object, const rsl::string &meta_method = "appendPlainText")
{
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
}

template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_st(
    const rsl::string &logger_name, QPlainTextEdit *qt_object, const rsl::string &meta_method = "appendPlainText")
{
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
}

template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_mt(const rsl::string &logger_name, QObject *qt_object, const rsl::string &meta_method)
{
    return Factory::template create<sinks::qt_sink_mt>(logger_name, qt_object, meta_method);
}

template<typename Factory = rexlog::synchronous_factory>
inline rsl::shared_ptr<logger> qt_logger_st(const rsl::string &logger_name, QObject *qt_object, const rsl::string &meta_method)
{
    return Factory::template create<sinks::qt_sink_st>(logger_name, qt_object, meta_method);
}
} // namespace rexlog
