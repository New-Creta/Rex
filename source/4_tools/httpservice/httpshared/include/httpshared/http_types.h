#pragma once

#include "rex_std/memory.h"
#include "rex_std/string_view.h"
#include "rex_std/unordered_map.h"
#include "rex_std/map.h"
#include "rex_std/ostream.h"
#include "rex_std/streambuf.h"
#include "rex_std/functional.h"
#include "rex_std/bonus/functional.h"

#include "rex_engine/engine/types.h"

#include "httpshared/http_error.h"
#include "httpshared/http_response.h"

#ifdef REX_PLATFORM_WINDOWS
#include <WinSock2.h>
#endif

namespace http
{
#define CPPHTTPLIB_VERSION "0.20.1"

#ifndef CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND
#define CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_KEEPALIVE_TIMEOUT_CHECK_INTERVAL_USECOND
#define CPPHTTPLIB_KEEPALIVE_TIMEOUT_CHECK_INTERVAL_USECOND 10000
#endif

#ifndef CPPHTTPLIB_KEEPALIVE_MAX_COUNT
#define CPPHTTPLIB_KEEPALIVE_MAX_COUNT 100
#endif

#ifndef CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND
#define CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND 300
#endif

#ifndef CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND
#define CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_SERVER_READ_TIMEOUT_SECOND
#define CPPHTTPLIB_SERVER_READ_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_SERVER_READ_TIMEOUT_USECOND
#define CPPHTTPLIB_SERVER_READ_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_SERVER_WRITE_TIMEOUT_SECOND
#define CPPHTTPLIB_SERVER_WRITE_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_SERVER_WRITE_TIMEOUT_USECOND
#define CPPHTTPLIB_SERVER_WRITE_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_CLIENT_READ_TIMEOUT_SECOND
#define CPPHTTPLIB_CLIENT_READ_TIMEOUT_SECOND 300
#endif

#ifndef CPPHTTPLIB_CLIENT_READ_TIMEOUT_USECOND
#define CPPHTTPLIB_CLIENT_READ_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_SECOND
#define CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_SECOND 5
#endif

#ifndef CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_USECOND
#define CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_USECOND 0
#endif

#ifndef CPPHTTPLIB_CLIENT_MAX_TIMEOUT_MSECOND
#define CPPHTTPLIB_CLIENT_MAX_TIMEOUT_MSECOND 0
#endif

#ifndef CPPHTTPLIB_IDLE_INTERVAL_SECOND
#define CPPHTTPLIB_IDLE_INTERVAL_SECOND 0
#endif

#ifndef CPPHTTPLIB_IDLE_INTERVAL_USECOND
#ifdef _WIN32
#define CPPHTTPLIB_IDLE_INTERVAL_USECOND 10000
#else
#define CPPHTTPLIB_IDLE_INTERVAL_USECOND 0
#endif
#endif

#ifndef CPPHTTPLIB_REQUEST_URI_MAX_LENGTH
#define CPPHTTPLIB_REQUEST_URI_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_HEADER_MAX_LENGTH
#define CPPHTTPLIB_HEADER_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_REDIRECT_MAX_COUNT
#define CPPHTTPLIB_REDIRECT_MAX_COUNT 20
#endif

#ifndef CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT
#define CPPHTTPLIB_MULTIPART_FORM_DATA_FILE_MAX_COUNT 1024
#endif

#ifndef CPPHTTPLIB_PAYLOAD_MAX_LENGTH
#define CPPHTTPLIB_PAYLOAD_MAX_LENGTH ((rsl::numeric_limits<size_t>::max)())
#endif

#ifndef CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH
#define CPPHTTPLIB_FORM_URL_ENCODED_PAYLOAD_MAX_LENGTH 8192
#endif

#ifndef CPPHTTPLIB_RANGE_MAX_COUNT
#define CPPHTTPLIB_RANGE_MAX_COUNT 1024
#endif

#ifndef CPPHTTPLIB_TCP_NODELAY
#define CPPHTTPLIB_TCP_NODELAY false
#endif

#ifndef CPPHTTPLIB_IPV6_V6ONLY
#define CPPHTTPLIB_IPV6_V6ONLY false
#endif

#ifndef CPPHTTPLIB_RECV_BUFSIZ
#define CPPHTTPLIB_RECV_BUFSIZ size_t(16384u)
#endif

#ifndef CPPHTTPLIB_COMPRESSION_BUFSIZ
#define CPPHTTPLIB_COMPRESSION_BUFSIZ size_t(16384u)
#endif

#ifndef CPPHTTPLIB_THREAD_POOL_COUNT
#define CPPHTTPLIB_THREAD_POOL_COUNT                                           \
  ((rsl::max)(8, rsl::thread::hardware_concurrency() > 0                      \
                      ? rsl::thread::hardware_concurrency() - 1                \
                      : 0))
#endif

#ifndef CPPHTTPLIB_RECV_FLAGS
#define CPPHTTPLIB_RECV_FLAGS 0
#endif

#ifndef CPPHTTPLIB_SEND_FLAGS
#define CPPHTTPLIB_SEND_FLAGS 0
#endif

#ifndef CPPHTTPLIB_LISTEN_BACKLOG
#define CPPHTTPLIB_LISTEN_BACKLOG 5
#endif

#ifndef CPPHTTPLIB_MAX_LINE_LENGTH
#define CPPHTTPLIB_MAX_LINE_LENGTH 32768
#endif



#ifdef REX_PLATFORM_WINDOWS
  using socket_t = SOCKET;
#endif

  using Headers = rsl::unordered_multimap<
    rsl::string,                                    // key 
    rsl::string,                                    // value
    rsl::hash_lower<rsl::string>,                   // hash
    rsl::equal_to_case_insensitive<rsl::string>     // comparison
  >;   
  using Progress = rsl::function<bool(uint64_t current, uint64_t total)>;
  using SocketOptions = rsl::function<void(socket_t sock)>;

  class Result {
  public:
    Result() = default;
    Result(rsl::unique_ptr<Response>&& res, Error err, Headers&& request_headers = Headers{})
      : res_(rsl::move(res))
      , err_(err)
      , request_headers_(rsl::move(request_headers)) 
    {}

    // Response
    operator bool() const { return res_ != nullptr; }
    bool operator==(rsl::nullptr_t) const { return res_ == nullptr; }
    bool operator!=(rsl::nullptr_t) const { return res_ != nullptr; }
    const Response& value() const { return *res_; }
    Response& value() { return *res_; }
    const Response& operator*() const { return *res_; }
    Response& operator*() { return *res_; }
    const Response* operator->() const { return res_.get(); }
    Response* operator->() { return res_.get(); }

    // Error
    Error error() const { return err_; }

    // Request Headers
    bool has_request_header(rsl::string_view key) const;
    rsl::string get_request_header_value(rsl::string_view key,
      const char* def = "",
      size_t id = 0) const;
    uint64_t get_request_header_value_u64(rsl::string_view key,
      uint64_t def = 0, size_t id = 0) const;
    size_t get_request_header_value_count(rsl::string_view key) const;

  private:
    rsl::unique_ptr<Response> res_;
    Error err_ = Error::Unknown;
    Headers request_headers_;
  };

  enum SSLVerifierResponse {
    // no decision has been made, use the built-in certificate verifier
    NoDecisionMade,
    // connection certificate is verified and accepted
    CertificateAccepted,
    // connection certificate was processed but is rejected
    CertificateRejected
  };

  enum StatusCode {
    // Information responses
    Continue_100 = 100,
    SwitchingProtocol_101 = 101,
    Processing_102 = 102,
    EarlyHints_103 = 103,

    // Successful responses
    OK_200 = 200,
    Created_201 = 201,
    Accepted_202 = 202,
    NonAuthoritativeInformation_203 = 203,
    NoContent_204 = 204,
    ResetContent_205 = 205,
    PartialContent_206 = 206,
    MultiStatus_207 = 207,
    AlreadyReported_208 = 208,
    IMUsed_226 = 226,

    // Redirection messages
    MultipleChoices_300 = 300,
    MovedPermanently_301 = 301,
    Found_302 = 302,
    SeeOther_303 = 303,
    NotModified_304 = 304,
    UseProxy_305 = 305,
    unused_306 = 306,
    TemporaryRedirect_307 = 307,
    PermanentRedirect_308 = 308,

    // Client error responses
    BadRequest_400 = 400,
    Unauthorized_401 = 401,
    PaymentRequired_402 = 402,
    Forbidden_403 = 403,
    NotFound_404 = 404,
    MethodNotAllowed_405 = 405,
    NotAcceptable_406 = 406,
    ProxyAuthenticationRequired_407 = 407,
    RequestTimeout_408 = 408,
    Conflict_409 = 409,
    Gone_410 = 410,
    LengthRequired_411 = 411,
    PreconditionFailed_412 = 412,
    PayloadTooLarge_413 = 413,
    UriTooLong_414 = 414,
    UnsupportedMediaType_415 = 415,
    RangeNotSatisfiable_416 = 416,
    ExpectationFailed_417 = 417,
    ImATeapot_418 = 418,
    MisdirectedRequest_421 = 421,
    UnprocessableContent_422 = 422,
    Locked_423 = 423,
    FailedDependency_424 = 424,
    TooEarly_425 = 425,
    UpgradeRequired_426 = 426,
    PreconditionRequired_428 = 428,
    TooManyRequests_429 = 429,
    RequestHeaderFieldsTooLarge_431 = 431,
    UnavailableForLegalReasons_451 = 451,

    // Server error responses
    InternalServerError_500 = 500,
    NotImplemented_501 = 501,
    BadGateway_502 = 502,
    ServiceUnavailable_503 = 503,
    GatewayTimeout_504 = 504,
    HttpVersionNotSupported_505 = 505,
    VariantAlsoNegotiates_506 = 506,
    InsufficientStorage_507 = 507,
    LoopDetected_508 = 508,
    NotExtended_510 = 510,
    NetworkAuthenticationRequired_511 = 511,
  };

  using Params = rsl::multimap<rsl::string, rsl::string>;
  using Match = std::smatch;

  struct Response;
  using ResponseHandler = rsl::function<bool(const Response& response)>;

  struct MultipartFormData {
    rsl::string name;
    rsl::string content;
    rsl::string filename;
    rsl::string content_type;
  };
  using MultipartFormDataItems = rsl::vector<MultipartFormData>;
  using MultipartFormDataMap = rsl::multimap<rsl::string, MultipartFormData>;

  class DataSink {
  public:
    DataSink() : os(&sb_), sb_(*this) {}

    DataSink(const DataSink&) = delete;
    DataSink& operator=(const DataSink&) = delete;
    DataSink(DataSink&&) = delete;
    DataSink& operator=(DataSink&&) = delete;

    rsl::function<bool(const char* data, size_t data_len)> write;
    rsl::function<bool()> is_writable;
    rsl::function<void()> done;
    rsl::function<void(const Headers& trailer)> done_with_trailer;
    rsl::ostream os;

  private:
    class data_sink_streambuf final : public rsl::streambuf {
    public:
      explicit data_sink_streambuf(DataSink& sink) : sink_(sink) {}

    protected:
      rsl::streamsize xsputn(const char* s, rsl::streamsize n) override {
        sink_.write(s, static_cast<size_t>(n));
        return n;
      }

    private:
      DataSink& sink_;
    };

    data_sink_streambuf sb_;
  };

  using ContentProvider =
    rsl::function<bool(size_t offset, size_t length, DataSink& sink)>;

  using ContentProviderWithoutLength =
    rsl::function<bool(size_t offset, DataSink& sink)>;

  using ContentProviderResourceReleaser = rsl::function<void(bool success)>;

  struct MultipartFormDataProvider {
    rsl::string name;
    ContentProviderWithoutLength provider;
    rsl::string filename;
    rsl::string content_type;
  };
  using MultipartFormDataProviderItems = rsl::vector<MultipartFormDataProvider>;

  using ContentReceiverWithProgress =
    rsl::function<bool(const char* data, size_t data_length, uint64_t offset,
      uint64_t total_length)>;

  using ContentReceiver =
    rsl::function<bool(const char* data, size_t data_length)>;

  using MultipartContentHeader =
    rsl::function<bool(const MultipartFormData& file)>;

  class ContentReader {
  public:
    using Reader = rsl::function<bool(ContentReceiver receiver)>;
    using MultipartReader = rsl::function<bool(MultipartContentHeader header,
      ContentReceiver receiver)>;

    ContentReader(Reader reader, MultipartReader multipart_reader)
      : reader_(rsl::move(reader)),
      multipart_reader_(rsl::move(multipart_reader)) {}

    bool operator()(MultipartContentHeader header,
      ContentReceiver receiver) const {
      return multipart_reader_(rsl::move(header), rsl::move(receiver));
    }

    bool operator()(ContentReceiver receiver) const {
      return reader_(rsl::move(receiver));
    }

    Reader reader_;
    MultipartReader multipart_reader_;
  };

  using Range = rsl::pair<s32, s32>;
  using Ranges = rsl::vector<Range>;

  class Stream {
  public:
    virtual ~Stream() = default;

    virtual bool is_readable() const = 0;
    virtual bool wait_readable() const = 0;
    virtual bool wait_writable() const = 0;

    virtual s32 read(char* ptr, size_t size) = 0;
    virtual s32 write(const char* ptr, size_t size) = 0;
    virtual void get_remote_ip_and_port(rsl::string& ip, int& port) const = 0;
    virtual void get_local_ip_and_port(rsl::string& ip, int& port) const = 0;
    virtual socket_t socket() const = 0;

    virtual time_t duration() const = 0;

    s32 write(const char* ptr);
    s32 write(rsl::string_view s);
  };
}