#pragma once

namespace http
{
  class ClientImpl {
  public:
    explicit ClientImpl(rsl::string_view host);

    explicit ClientImpl(rsl::string_view host, int port);

    explicit ClientImpl(rsl::string_view host, int port,
      rsl::string_view client_cert_path,
      rsl::string_view client_key_path);

    virtual ~ClientImpl();

    virtual bool is_valid() const;

    Result Get(rsl::string_view path);
    Result Get(rsl::string_view path, const Headers& headers);
    Result Get(rsl::string_view path, Progress progress);
    Result Get(rsl::string_view path, const Headers& headers,
      Progress progress);
    Result Get(rsl::string_view path, ContentReceiver content_receiver);
    Result Get(rsl::string_view path, const Headers& headers,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, ContentReceiver content_receiver,
      Progress progress);
    Result Get(rsl::string_view path, const Headers& headers,
      ContentReceiver content_receiver, Progress progress);
    Result Get(rsl::string_view path, ResponseHandler response_handler,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, const Headers& headers,
      ResponseHandler response_handler,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, ResponseHandler response_handler,
      ContentReceiver content_receiver, Progress progress);
    Result Get(rsl::string_view path, const Headers& headers,
      ResponseHandler response_handler, ContentReceiver content_receiver,
      Progress progress);

    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, Progress progress = nullptr);
    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, ContentReceiver content_receiver,
      Progress progress = nullptr);
    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, ResponseHandler response_handler,
      ContentReceiver content_receiver, Progress progress = nullptr);

    Result Head(rsl::string_view path);
    Result Head(rsl::string_view path, const Headers& headers);

    Result Post(rsl::string_view path);
    Result Post(rsl::string_view path, const Headers& headers);
    Result Post(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Post(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Post(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Post(rsl::string_view path, size_t content_length,
      ContentProvider content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Params& params);
    Result Post(rsl::string_view path, const Headers& headers,
      const Params& params);
    Result Post(rsl::string_view path, const Headers& headers,
      const Params& params, Progress progress);
    Result Post(rsl::string_view path, const MultipartFormDataItems& items);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items, rsl::string_view boundary);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items,
      const MultipartFormDataProviderItems& provider_items);

    Result Put(rsl::string_view path);
    Result Put(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Put(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Put(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Put(rsl::string_view path, size_t content_length,
      ContentProvider content_provider, rsl::string_view content_type);
    Result Put(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Params& params);
    Result Put(rsl::string_view path, const Headers& headers,
      const Params& params);
    Result Put(rsl::string_view path, const Headers& headers,
      const Params& params, Progress progress);
    Result Put(rsl::string_view path, const MultipartFormDataItems& items);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items, rsl::string_view boundary);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items,
      const MultipartFormDataProviderItems& provider_items);

    Result Patch(rsl::string_view path);
    Result Patch(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Patch(rsl::string_view path, size_t content_length,
      ContentProvider content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);

    Result Delete(rsl::string_view path);
    Result Delete(rsl::string_view path, const Headers& headers);
    Result Delete(rsl::string_view path, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Delete(rsl::string_view path, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Delete(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Delete(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Delete(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Delete(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Delete(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Delete(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);

    Result Options(rsl::string_view path);
    Result Options(rsl::string_view path, const Headers& headers);

    bool send(Request& req, Response& res, Error& error);
    Result send(const Request& req);

    void stop();

    rsl::string host() const;
    int port() const;

    size_t is_socket_open() const;
    socket_t socket() const;

    void set_hostname_addr_map(rsl::map<rsl::string, rsl::string> addr_map);

    void set_default_headers(Headers headers);

    void
      set_header_writer(rsl::function<ssize_t(Stream&, Headers&)> const& writer);

    void set_address_family(int family);
    void set_tcp_nodelay(bool on);
    void set_ipv6_v6only(bool on);
    void set_socket_options(SocketOptions socket_options);

    void set_connection_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void
      set_connection_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_read_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void set_read_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_write_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void set_write_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_max_timeout(time_t msec);
    template <class Rep, class Period>
    void set_max_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_basic_auth(rsl::string_view username, rsl::string_view password);
    void set_bearer_token_auth(rsl::string_view token);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_digest_auth(rsl::string_view username,
      rsl::string_view password);
#endif

    void set_keep_alive(bool on);
    void set_follow_location(bool on);

    void set_url_encode(bool on);

    void set_compress(bool on);

    void set_decompress(bool on);

    void set_interface(rsl::string_view intf);

    void set_proxy(rsl::string_view host, int port);
    void set_proxy_basic_auth(rsl::string_view username,
      rsl::string_view password);
    void set_proxy_bearer_token_auth(rsl::string_view token);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_proxy_digest_auth(rsl::string_view username,
      rsl::string_view password);
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_ca_cert_path(rsl::string_view ca_cert_file_path,
      rsl::string_view ca_cert_dir_path = rsl::string());
    void set_ca_cert_store(X509_STORE* ca_cert_store);
    X509_STORE* create_ca_cert_store(const char* ca_cert, rsl::size_t size) const;
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void enable_server_certificate_verification(bool enabled);
    void enable_server_hostname_verification(bool enabled);
    void set_server_certificate_verifier(
      rsl::function<SSLVerifierResponse(SSL* ssl)> verifier);
#endif

    void set_logger(Logger logger);

  protected:
    struct Socket {
      socket_t sock = INVALID_SOCKET;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
      SSL* ssl = nullptr;
#endif

      bool is_open() const { return sock != INVALID_SOCKET; }
    };

    virtual bool create_and_connect_socket(Socket& socket, Error& error);

    // All of:
    //   shutdown_ssl
    //   shutdown_socket
    //   close_socket
    // should ONLY be called when socket_mutex_ is locked.
    // Also, shutdown_ssl and close_socket should also NOT be called concurrently
    // with a DIFFERENT thread sending requests using that socket.
    virtual void shutdown_ssl(Socket& socket, bool shutdown_gracefully);
    void shutdown_socket(Socket& socket) const;
    void close_socket(Socket& socket);

    bool process_request(Stream& strm, Request& req, Response& res,
      bool close_connection, Error& error);

    bool write_content_with_provider(Stream& strm, const Request& req,
      Error& error) const;

    void copy_settings(const ClientImpl& rhs);

    // Socket endpoint information
    const rsl::string host_;
    const int port_;
    const rsl::string host_and_port_;

    // Current open socket
    Socket socket_;
    mutable rsl::mutex socket_mutex_;
    rsl::recursive_mutex request_mutex_;

    // These are all protected under socket_mutex
    size_t socket_requests_in_flight_ = 0;
    rsl::thread::id socket_requests_are_from_thread_ = rsl::thread::id(-1);
    bool socket_should_be_closed_when_request_is_done_ = false;

    // Hostname-IP map
    rsl::map<rsl::string, rsl::string> addr_map_;

    // Default headers
    Headers default_headers_;

    // Header writer
    rsl::function<ssize_t(Stream&, Headers&)> header_writer_ =
      detail::write_headers;

    // Settings
    rsl::string client_cert_path_;
    rsl::string client_key_path_;

    time_t connection_timeout_sec_ = CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND;
    time_t connection_timeout_usec_ = CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND;
    time_t read_timeout_sec_ = CPPHTTPLIB_CLIENT_READ_TIMEOUT_SECOND;
    time_t read_timeout_usec_ = CPPHTTPLIB_CLIENT_READ_TIMEOUT_USECOND;
    time_t write_timeout_sec_ = CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_SECOND;
    time_t write_timeout_usec_ = CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_USECOND;
    time_t max_timeout_msec_ = CPPHTTPLIB_CLIENT_MAX_TIMEOUT_MSECOND;

    rsl::string basic_auth_username_;
    rsl::string basic_auth_password_;
    rsl::string bearer_token_auth_token_;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string digest_auth_username_;
    rsl::string digest_auth_password_;
#endif

    bool keep_alive_ = false;
    bool follow_location_ = false;

    bool url_encode_ = true;

    int address_family_ = AF_UNSPEC;
    bool tcp_nodelay_ = CPPHTTPLIB_TCP_NODELAY;
    bool ipv6_v6only_ = CPPHTTPLIB_IPV6_V6ONLY;
    SocketOptions socket_options_ = nullptr;

    bool compress_ = false;
    bool decompress_ = true;

    rsl::string interface_;

    rsl::string proxy_host_;
    int proxy_port_ = -1;

    rsl::string proxy_basic_auth_username_;
    rsl::string proxy_basic_auth_password_;
    rsl::string proxy_bearer_token_auth_token_;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string proxy_digest_auth_username_;
    rsl::string proxy_digest_auth_password_;
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string ca_cert_file_path_;
    rsl::string ca_cert_dir_path_;

    X509_STORE* ca_cert_store_ = nullptr;
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    bool server_certificate_verification_ = true;
    bool server_hostname_verification_ = true;
    rsl::function<SSLVerifierResponse(SSL* ssl)> server_certificate_verifier_;
#endif

    Logger logger_;

  private:
    bool send_(Request& req, Response& res, Error& error);
    Result send_(Request&& req);

    socket_t create_client_socket(Error& error) const;
    bool read_response_line(Stream& strm, const Request& req,
      Response& res) const;
    bool write_request(Stream& strm, Request& req, bool close_connection,
      Error& error);
    bool redirect(Request& req, Response& res, Error& error);
    bool handle_request(Stream& strm, Request& req, Response& res,
      bool close_connection, Error& error);
    rsl::unique_ptr<Response> send_with_content_provider(
      Request& req, const char* body, size_t content_length,
      ContentProvider content_provider,
      ContentProviderWithoutLength content_provider_without_length,
      rsl::string_view content_type, Error& error);
    Result send_with_content_provider(
      rsl::string_view method, rsl::string_view path,
      const Headers& headers, const char* body, size_t content_length,
      ContentProvider content_provider,
      ContentProviderWithoutLength content_provider_without_length,
      rsl::string_view content_type, Progress progress);
    ContentProviderWithoutLength get_multipart_content_provider(
      rsl::string_view boundary, const MultipartFormDataItems& items,
      const MultipartFormDataProviderItems& provider_items) const;

    rsl::string adjust_host_string(rsl::string_view host) const;

    virtual bool
      process_socket(const Socket& socket,
        rsl::chrono::time_point<rsl::chrono::steady_clock> start_time,
        rsl::function<bool(Stream& strm)> callback);
    virtual bool is_ssl() const;
  };

  class Client {
  public:
    // Universal interface
    explicit Client(rsl::string_view scheme_host_port);

    explicit Client(rsl::string_view scheme_host_port,
      rsl::string_view client_cert_path,
      rsl::string_view client_key_path);

    // HTTP only interface
    explicit Client(rsl::string_view host, int port);

    explicit Client(rsl::string_view host, int port,
      rsl::string_view client_cert_path,
      rsl::string_view client_key_path);

    Client(Client&&) = default;
    Client& operator=(Client&&) = default;

    ~Client();

    bool is_valid() const;

    Result Get(rsl::string_view path);
    Result Get(rsl::string_view path, const Headers& headers);
    Result Get(rsl::string_view path, Progress progress);
    Result Get(rsl::string_view path, const Headers& headers,
      Progress progress);
    Result Get(rsl::string_view path, ContentReceiver content_receiver);
    Result Get(rsl::string_view path, const Headers& headers,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, ContentReceiver content_receiver,
      Progress progress);
    Result Get(rsl::string_view path, const Headers& headers,
      ContentReceiver content_receiver, Progress progress);
    Result Get(rsl::string_view path, ResponseHandler response_handler,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, const Headers& headers,
      ResponseHandler response_handler,
      ContentReceiver content_receiver);
    Result Get(rsl::string_view path, const Headers& headers,
      ResponseHandler response_handler, ContentReceiver content_receiver,
      Progress progress);
    Result Get(rsl::string_view path, ResponseHandler response_handler,
      ContentReceiver content_receiver, Progress progress);

    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, Progress progress = nullptr);
    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, ContentReceiver content_receiver,
      Progress progress = nullptr);
    Result Get(rsl::string_view path, const Params& params,
      const Headers& headers, ResponseHandler response_handler,
      ContentReceiver content_receiver, Progress progress = nullptr);

    Result Head(rsl::string_view path);
    Result Head(rsl::string_view path, const Headers& headers);

    Result Post(rsl::string_view path);
    Result Post(rsl::string_view path, const Headers& headers);
    Result Post(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Post(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Post(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Post(rsl::string_view path, size_t content_length,
      ContentProvider content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Post(rsl::string_view path, const Params& params);
    Result Post(rsl::string_view path, const Headers& headers,
      const Params& params);
    Result Post(rsl::string_view path, const Headers& headers,
      const Params& params, Progress progress);
    Result Post(rsl::string_view path, const MultipartFormDataItems& items);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items, rsl::string_view boundary);
    Result Post(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items,
      const MultipartFormDataProviderItems& provider_items);

    Result Put(rsl::string_view path);
    Result Put(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Put(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Put(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Put(rsl::string_view path, size_t content_length,
      ContentProvider content_provider, rsl::string_view content_type);
    Result Put(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Put(rsl::string_view path, const Params& params);
    Result Put(rsl::string_view path, const Headers& headers,
      const Params& params);
    Result Put(rsl::string_view path, const Headers& headers,
      const Params& params, Progress progress);
    Result Put(rsl::string_view path, const MultipartFormDataItems& items);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items, rsl::string_view boundary);
    Result Put(rsl::string_view path, const Headers& headers,
      const MultipartFormDataItems& items,
      const MultipartFormDataProviderItems& provider_items);

    Result Patch(rsl::string_view path);
    Result Patch(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Patch(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);
    Result Patch(rsl::string_view path, size_t content_length,
      ContentProvider content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      size_t content_length, ContentProvider content_provider,
      rsl::string_view content_type);
    Result Patch(rsl::string_view path, const Headers& headers,
      ContentProviderWithoutLength content_provider,
      rsl::string_view content_type);

    Result Delete(rsl::string_view path);
    Result Delete(rsl::string_view path, const Headers& headers);
    Result Delete(rsl::string_view path, const char* body,
      size_t content_length, rsl::string_view content_type);
    Result Delete(rsl::string_view path, const char* body,
      size_t content_length, rsl::string_view content_type,
      Progress progress);
    Result Delete(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type);
    Result Delete(rsl::string_view path, const Headers& headers,
      const char* body, size_t content_length,
      rsl::string_view content_type, Progress progress);
    Result Delete(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type);
    Result Delete(rsl::string_view path, rsl::string_view body,
      rsl::string_view content_type, Progress progress);
    Result Delete(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type);
    Result Delete(rsl::string_view path, const Headers& headers,
      rsl::string_view body, rsl::string_view content_type,
      Progress progress);

    Result Options(rsl::string_view path);
    Result Options(rsl::string_view path, const Headers& headers);

    bool send(Request& req, Response& res, Error& error);
    Result send(const Request& req);

    void stop();

    rsl::string host() const;
    int port() const;

    size_t is_socket_open() const;
    socket_t socket() const;

    void set_hostname_addr_map(rsl::map<rsl::string, rsl::string> addr_map);

    void set_default_headers(Headers headers);

    void
      set_header_writer(rsl::function<ssize_t(Stream&, Headers&)> const& writer);

    void set_address_family(int family);
    void set_tcp_nodelay(bool on);
    void set_socket_options(SocketOptions socket_options);

    void set_connection_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void
      set_connection_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_read_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void set_read_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_write_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void set_write_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_max_timeout(time_t msec);
    template <class Rep, class Period>
    void set_max_timeout(const rsl::chrono::duration<Rep, Period>& duration);

    void set_basic_auth(rsl::string_view username, rsl::string_view password);
    void set_bearer_token_auth(rsl::string_view token);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_digest_auth(rsl::string_view username,
      rsl::string_view password);
#endif

    void set_keep_alive(bool on);
    void set_follow_location(bool on);

    void set_url_encode(bool on);

    void set_compress(bool on);

    void set_decompress(bool on);

    void set_interface(rsl::string_view intf);

    void set_proxy(rsl::string_view host, int port);
    void set_proxy_basic_auth(rsl::string_view username,
      rsl::string_view password);
    void set_proxy_bearer_token_auth(rsl::string_view token);
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_proxy_digest_auth(rsl::string_view username,
      rsl::string_view password);
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void enable_server_certificate_verification(bool enabled);
    void enable_server_hostname_verification(bool enabled);
    void set_server_certificate_verifier(
      rsl::function<SSLVerifierResponse(SSL* ssl)> verifier);
#endif

    void set_logger(Logger logger);

    // SSL
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    void set_ca_cert_path(rsl::string_view ca_cert_file_path,
      rsl::string_view ca_cert_dir_path = rsl::string());

    void set_ca_cert_store(X509_STORE* ca_cert_store);
    void load_ca_cert_store(const char* ca_cert, rsl::size_t size);

    long get_openssl_verify_result() const;

    SSL_CTX* ssl_context() const;
#endif

  private:
    rsl::unique_ptr<ClientImpl> cli_;

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    bool is_ssl_ = false;
#endif
  };

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
  class SSLClient final : public ClientImpl {
  public:
    explicit SSLClient(const std::string& host);

    explicit SSLClient(const std::string& host, int port);

    explicit SSLClient(const std::string& host, int port,
      const std::string& client_cert_path,
      const std::string& client_key_path,
      const std::string& private_key_password = std::string());

    explicit SSLClient(const std::string& host, int port, X509* client_cert,
      EVP_PKEY* client_key,
      const std::string& private_key_password = std::string());

    ~SSLClient() override;

    bool is_valid() const override;

    void set_ca_cert_store(X509_STORE* ca_cert_store);
    void load_ca_cert_store(const char* ca_cert, std::size_t size);

    long get_openssl_verify_result() const;

    SSL_CTX* ssl_context() const;

  private:
    bool create_and_connect_socket(Socket& socket, Error& error) override;
    void shutdown_ssl(Socket& socket, bool shutdown_gracefully) override;
    void shutdown_ssl_impl(Socket& socket, bool shutdown_gracefully);

    bool process_socket(const Socket& socket,
      std::function<bool(Stream& strm)> callback) override;
    bool is_ssl() const override;

    bool connect_with_proxy(Socket& sock, Response& res, bool& success,
      Error& error);
    bool initialize_ssl(Socket& socket, Error& error);

    bool load_certs();

    bool verify_host(X509* server_cert) const;
    bool verify_host_with_subject_alt_name(X509* server_cert) const;
    bool verify_host_with_common_name(X509* server_cert) const;
    bool check_host_name(const char* pattern, size_t pattern_len) const;

    SSL_CTX* ctx_;
    std::mutex ctx_mutex_;
    std::once_flag initialize_cert_;

    std::vector<std::string> host_components_;

    long verify_result_ = 0;

    friend class ClientImpl;
  };
#endif
}