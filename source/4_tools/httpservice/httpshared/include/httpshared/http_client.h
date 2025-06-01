#pragma once

#include "rex_std/string_view.h"
#include "rex_std/thread.h"
#include "rex_std/mutex.h"

#include "httpshared/http_types.h"
#include "httpshared/http_request.h"

namespace http
{
  struct ClientDesc
  {
    rsl::string scheme;
    rsl::string host;
    rsl::string cert_path;
    rsl::string key_path;
    s32 port = 80; // for http, port is 80, for https, port is 443
  };

  class IContentProvider
  {
  public:
    virtual void read(void* data, s32 size) = 0;
  };

  enum class HttpMethod
  {
    Get,
    Head,
    Post,
    Put,
    Patch,
    Delete,
    Options
  };

  struct SendRequestDesc
  {
    rsl::string path;
    Headers headers;
    HttpMethod method;
    Progress progress;
    rsl::string body;
    IContentProvider* content_provider;
  };
  struct GetRequestDesc
  {
    rsl::string path;
    Headers headers;
    Params params;

    Progress progress;
    ContentReceiver content_receiver;
    ResponseHandler response_handler;
  };
  struct HeadRequestDesc
  {
    rsl::string_view path;
    Headers headers;
  };
  struct PostRequestDesc : public SendRequestDesc
  {
    PostRequestDesc()
    {
      method = HttpMethod::Post;
    }

    rsl::string_view content_type;
    Params params;
    MultipartFormDataItems items;
    rsl::string boundary;
    MultipartFormDataProviderItems provider_items;
  };
  struct PutRequestDesc : public SendRequestDesc
  {
    PutRequestDesc()
    {
      method = HttpMethod::Put;
    }

    rsl::string_view content_type;
    Params params;
  };
  struct PatchRequestDesc : public SendRequestDesc
  {
    PatchRequestDesc()
    {
      method = HttpMethod::Patch;
    }

    rsl::string_view content_type;
    Params params;
  };
  struct DeleteRequestDesc : public SendRequestDesc
  {
    DeleteRequestDesc()
    {
      method = HttpMethod::Delete;
    }

    rsl::string_view content_type;
    Params params;
  };
  struct OptionsRequestDesc : public SendRequestDesc
  {
    OptionsRequestDesc()
    {
      method = HttpMethod::Options;
    }

    rsl::string_view path;
    Headers headers;
  };

  class ClientBase 
  {
  public:
    explicit ClientBase(const ClientDesc& desc);
    virtual ~ClientBase();

    virtual bool is_valid() const;

    // HTTP requests
    Result get(const GetRequestDesc& desc);
    Result head(const HeadRequestDesc& desc);
    Result post(const PostRequestDesc& desc);
    Result put(const PutRequestDesc& desc);
    Result patch(const PatchRequestDesc& desc);
    Result del(const DeleteRequestDesc& desc);
    Result options(const OptionsRequestDesc& desc);

    // Getters
    rsl::string_view host() const;
    int port() const;

    bool send(Request& req, Response& res, Error& error);
    Result send(const Request& req);

    void stop();

    size_t is_socket_open() const;
    socket_t socket() const;

    void set_hostname_addr_map(rsl::map<rsl::string, rsl::string> addr_map);
    void set_default_headers(Headers headers);

    void set_header_writer(rsl::function<s32(Stream&, Headers&)> const& writer);

    void set_address_family(int family);
    void set_tcp_nodelay(bool on);
    void set_ipv6_v6only(bool on);
    void set_socket_options(SocketOptions socket_options);

    void set_connection_timeout(time_t sec, time_t usec = 0);
    template <class Rep, class Period>
    void set_connection_timeout(const rsl::chrono::duration<Rep, Period>& duration);

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

    //void set_logger(Logger logger);

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

    void copy_settings(const ClientBase& rhs);

  private:
    void send_with_content_provider(const SendRequestDesc& desc);

  private:
    // Socket endpoint information
    const rsl::string m_host;
    const int m_port;
    const rsl::string m_host_and_port;

    // Current open socket
    Socket m_socket;
    mutable rsl::mutex m_socket_mutex;
    rsl::recursive_mutex m_request_mutex;

    // These are all protected under socket_mutex
    size_t m_socket_requests_in_flight = 0;
    rsl::thread::id m_socket_requests_are_from_thread = rsl::thread::id(-1);
    bool m_socket_should_be_closed_when_request_is_done = false;

    // Hostname-IP map
    rsl::map<rsl::string, rsl::string> m_addr_map;

    // Default headers
    Headers m_default_headers;

    // Header writer
    rsl::function<s32(Stream&, Headers&)> m_header_writer = detail::write_headers;

    // Settings
    rsl::string m_client_cert_path;
    rsl::string m_client_key_path;

    time_t m_connection_timeout_sec = CPPHTTPLIB_CONNECTION_TIMEOUT_SECOND;
    time_t m_connection_timeout_usec = CPPHTTPLIB_CONNECTION_TIMEOUT_USECOND;
    time_t m_read_timeout_sec = CPPHTTPLIB_CLIENT_READ_TIMEOUT_SECOND;
    time_t m_read_timeout_usec = CPPHTTPLIB_CLIENT_READ_TIMEOUT_USECOND;
    time_t m_write_timeout_sec = CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_SECOND;
    time_t m_write_timeout_usec = CPPHTTPLIB_CLIENT_WRITE_TIMEOUT_USECOND;
    time_t m_max_timeout_msec = CPPHTTPLIB_CLIENT_MAX_TIMEOUT_MSECOND;

    rsl::string m_basic_auth_username;
    rsl::string m_basic_auth_password;
    rsl::string m_bearer_token_auth_token;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string m_digest_auth_username;
    rsl::string m_digest_auth_password;
#endif

    bool m_keep_alive = false;
    bool m_follow_location = false;

    bool m_url_encode = true;

    int m_address_family = AF_UNSPEC;
    bool m_tcp_nodelay = CPPHTTPLIB_TCP_NODELAY;
    bool m_ipv6_v6only = CPPHTTPLIB_IPV6_V6ONLY;
    SocketOptions m_socket_options = nullptr;

    bool m_compress = false;
    bool m_decompress = true;

    rsl::string m_interface;

    rsl::string m_proxy_host;
    int m_proxy_port = -1;

    rsl::string m_proxy_basic_auth_username;
    rsl::string m_proxy_basic_auth_password;
    rsl::string m_proxy_bearer_token_auth_token;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string m_proxy_digest_auth_username;
    rsl::string m_proxy_digest_auth_password;
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    rsl::string m_ca_cert_file_path;
    rsl::string m_ca_cert_dir_path;

    X509_STORE* m_ca_cert_store = nullptr;
#endif

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    bool m_server_certificate_verification = true;
    bool m_server_hostname_verification = true;
    rsl::function<SSLVerifierResponse(SSL* ssl)> m_server_certificate_verifier;
#endif

    //Logger logger_;

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

#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
  class SSLClient final : public ClientBase {
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

    friend class ClientBase;
  };
#endif
}