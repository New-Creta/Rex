#pragma once

#include "httpshared/http_request.h"
#include "httpshared/http_response.h"

#include "rex_std/functional.h"
#include "rex_std/string.h"
#include "rex_std/string_view.h"
#include "rex_std/atomic.h"
#include "rex_std/map.h"
#include "rex_std/memory.h"

namespace http
{
  class Server {
  public:
    using Handler = rsl::function<void(const Request&, Response&)>;

    using ExceptionHandler =
      rsl::function<void(const Request&, Response&)>;

    enum class HandlerResponse {
      Handled,
      Unhandled,
    };
    using HandlerWithResponse =
      rsl::function<HandlerResponse(const Request&, Response&)>;

    using HandlerWithContentReader = rsl::function<void(
      const Request&, Response&, const ContentReader& content_reader)>;

    using Expect100ContinueHandler =
      rsl::function<int(const Request&, Response&)>;

    Server();
    Server(const Server&) = delete;

    virtual ~Server();

    virtual bool is_valid() const;

    Server& Get(rsl::string_view pattern, Handler handler);
    Server& Post(rsl::string_view pattern, Handler handler);
    Server& Post(rsl::string_view pattern, HandlerWithContentReader handler);
    Server& Put(rsl::string_view pattern, Handler handler);
    Server& Put(rsl::string_view pattern, HandlerWithContentReader handler);
    Server& Patch(rsl::string_view pattern, Handler handler);
    Server& Patch(rsl::string_view pattern, HandlerWithContentReader handler);
    Server& Delete(rsl::string_view pattern, Handler handler);
    Server& Delete(rsl::string_view pattern, HandlerWithContentReader handler);
    Server& Options(rsl::string_view pattern, Handler handler);

    bool set_base_dir(rsl::string_view dir,
      rsl::string_view mount_point = rsl::string());
    bool set_mount_point(rsl::string_view mount_point, rsl::string_view dir,
      Headers headers = Headers());
    bool remove_mount_point(rsl::string_view mount_point);
    Server& set_file_extension_and_mimetype_mapping(rsl::string_view ext,
      rsl::string_view mime);
    Server& set_default_file_mimetype(rsl::string_view mime);
    Server& set_file_request_handler(Handler handler);

    template <class ErrorHandlerFunc>
    Server& set_error_handler(ErrorHandlerFunc&& handler) {
      return set_error_handler_core(
        rsl::forward<ErrorHandlerFunc>(handler),
        rsl::is_convertible<ErrorHandlerFunc, HandlerWithResponse>{});
    }

    Server& set_exception_handler(ExceptionHandler handler);
    Server& set_pre_routing_handler(HandlerWithResponse handler);
    Server& set_post_routing_handler(Handler handler);

    Server& set_expect_100_continue_handler(Expect100ContinueHandler handler);
    Server& set_logger(Logger logger);

    Server& set_address_family(int family);
    Server& set_tcp_nodelay(bool on);
    Server& set_ipv6_v6only(bool on);
    Server& set_socket_options(SocketOptions socket_options);

    Server& set_default_headers(Headers headers);
    Server&
      set_header_writer(rsl::function<ssize_t(Stream&, Headers&)> const& writer);

    Server& set_keep_alive_max_count(size_t count);
    Server& set_keep_alive_timeout(time_t sec);

    Server& set_read_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    Server& set_read_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    Server& set_write_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    Server& set_write_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    Server& set_idle_interval(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    Server& set_idle_interval(const rsl::chrono::duration<Rep, Period>& duration);

    Server& set_payload_max_length(size_t length);

    bool bind_to_port(rsl::string_view host, int port, int socket_flags = 0);
    int bind_to_any_port(rsl::string_view host, int socket_flags = 0);
    bool listen_after_bind();

    bool listen(rsl::string_view host, int port, int socket_flags = 0);

    bool is_running() const;
    void wait_until_ready() const;
    void stop();
    void decommission();

    rsl::function<TaskQueue* (void)> new_task_queue;

  protected:
    bool process_request(Stream& strm, rsl::string_view remote_addr,
      int remote_port, rsl::string_view local_addr,
      int local_port, bool close_connection,
      bool& connection_closed,
      const rsl::function<void(Request&)>& setup_request);

    rsl::atomic<socket_t> svr_sock_{ INVALID_SOCKET };
    size_t keep_alive_max_count_ = CPPHTTPLIB_KEEPALIVE_MAX_COUNT;
    time_t keep_alive_timeout_sec_ = CPPHTTPLIB_KEEPALIVE_TIMEOUT_SECOND;
    time_t read_timeout_sec_ = CPPHTTPLIB_SERVER_READ_TIMEOUT_SECOND;
    time_t read_timeout_usec_ = CPPHTTPLIB_SERVER_READ_TIMEOUT_USECOND;
    time_t write_timeout_sec_ = CPPHTTPLIB_SERVER_WRITE_TIMEOUT_SECOND;
    time_t write_timeout_usec_ = CPPHTTPLIB_SERVER_WRITE_TIMEOUT_USECOND;
    time_t idle_interval_sec_ = CPPHTTPLIB_IDLE_INTERVAL_SECOND;
    time_t idle_interval_usec_ = CPPHTTPLIB_IDLE_INTERVAL_USECOND;
    size_t payload_max_length_ = CPPHTTPLIB_PAYLOAD_MAX_LENGTH;

  private:
    using Handlers =
      rsl::vector<rsl::pair<rsl::unique_ptr<detail::MatcherBase>, Handler>>;
    using HandlersForContentReader =
      rsl::vector<rsl::pair<rsl::unique_ptr<detail::MatcherBase>,
      HandlerWithContentReader>>;

    static rsl::unique_ptr<detail::MatcherBase>
      make_matcher(rsl::string_view pattern);

    Server& set_error_handler_core(HandlerWithResponse handler, rsl::true_type);
    Server& set_error_handler_core(Handler handler, rsl::false_type);

    socket_t create_server_socket(rsl::string_view host, int port,
      int socket_flags,
      SocketOptions socket_options) const;
    int bind_internal(rsl::string_view host, int port, int socket_flags);
    bool listen_internal();

    bool routing(Request& req, Response& res, Stream& strm);
    bool handle_file_request(const Request& req, Response& res);
    bool dispatch_request(Request& req, Response& res,
      const Handlers& handlers) const;
    bool dispatch_request_for_content_reader(
      Request& req, Response& res, ContentReader content_reader,
      const HandlersForContentReader& handlers) const;

    bool parse_request_line(const char* s, Request& req) const;
    void apply_ranges(const Request& req, Response& res,
      rsl::string& content_type, rsl::string& boundary) const;
    bool write_response(Stream& strm, bool close_connection, Request& req,
      Response& res);
    bool write_response_with_content(Stream& strm, bool close_connection,
      const Request& req, Response& res);
    bool write_response_core(Stream& strm, bool close_connection,
      const Request& req, Response& res,
      bool need_apply_ranges);
    bool write_content_with_provider(Stream& strm, const Request& req,
      Response& res, rsl::string_view boundary,
      rsl::string_view content_type);
    bool read_content(Stream& strm, Request& req, Response& res);
    bool
      read_content_with_content_receiver(Stream& strm, Request& req, Response& res,
        ContentReceiver receiver,
        MultipartContentHeader multipart_header,
        ContentReceiver multipart_receiver);
    bool read_content_core(Stream& strm, Request& req, Response& res,
      ContentReceiver receiver,
      MultipartContentHeader multipart_header,
      ContentReceiver multipart_receiver) const;

    virtual bool process_and_close_socket(socket_t sock);

    rsl::atomic<bool> is_running_{ false };
    rsl::atomic<bool> is_decommissioned{ false };

    struct MountPointEntry {
      rsl::string mount_point;
      rsl::string base_dir;
      Headers headers;
    };
    rsl::vector<MountPointEntry> base_dirs_;
    rsl::map<rsl::string, rsl::string> file_extension_and_mimetype_map_;
    rsl::string_view default_file_mimetype_ = "application/octet-stream";
    Handler file_request_handler_;

    Handlers get_handlers_;
    Handlers post_handlers_;
    HandlersForContentReader post_handlers_for_content_reader_;
    Handlers put_handlers_;
    HandlersForContentReader put_handlers_for_content_reader_;
    Handlers patch_handlers_;
    HandlersForContentReader patch_handlers_for_content_reader_;
    Handlers delete_handlers_;
    HandlersForContentReader delete_handlers_for_content_reader_;
    Handlers options_handlers_;

    HandlerWithResponse error_handler_;
    ExceptionHandler exception_handler_;
    HandlerWithResponse pre_routing_handler_;
    Handler post_routing_handler_;
    Expect100ContinueHandler expect_100_continue_handler_;

    Logger logger_;

    int address_family_ = AF_UNSPEC;
    bool tcp_nodelay_ = CPPHTTPLIB_TCP_NODELAY;
    bool ipv6_v6only_ = CPPHTTPLIB_IPV6_V6ONLY;
    SocketOptions socket_options_ = default_socket_options;

    Headers default_headers_;
    rsl::function<ssize_t(Stream&, Headers&)> header_writer_ =
      detail::write_headers;
  };

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
  class SSLServer : public Server {
  public:
    SSLServer(const char* cert_path, const char* private_key_path,
      const char* client_ca_cert_file_path = nullptr,
      const char* client_ca_cert_dir_path = nullptr,
      const char* private_key_password = nullptr);

    SSLServer(X509* cert, EVP_PKEY* private_key,
      X509_STORE* client_ca_cert_store = nullptr);

    SSLServer(
      const std::function<bool(SSL_CTX& ssl_ctx)>& setup_ssl_ctx_callback);

    ~SSLServer() override;

    bool is_valid() const override;

    SSL_CTX* ssl_context() const;

    void update_certs(X509* cert, EVP_PKEY* private_key,
      X509_STORE* client_ca_cert_store = nullptr);

  private:
    bool process_and_close_socket(socket_t sock) override;

    SSL_CTX* ctx_;
    std::mutex ctx_mutex_;
  };
#endif
}