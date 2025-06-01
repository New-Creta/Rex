#include "httpshared/http_utils.h"

namespace http
{
  rsl::string escape_abstract_namespace_unix_domain(rsl::string_view s) {
    if (s.size() > 1 && s[0] == '\0') {
      rsl::string ret(s);
      ret[0] = '@';
      return ret;
    }
    return rsl::string(s);
  }

  socket_t create_client_socket(
    rsl::string_view host, rsl::string_view ip, int port,
    int address_family, bool tcp_nodelay, bool ipv6_v6only,
    SocketOptions socket_options, time_t connection_timeout_sec,
    time_t connection_timeout_usec, time_t read_timeout_sec,
    time_t read_timeout_usec, time_t write_timeout_sec,
    time_t write_timeout_usec, rsl::string_view intf, Error& error) {
    auto sock = create_socket(
      host, ip, port, address_family, 0, tcp_nodelay, ipv6_v6only,
      rsl::move(socket_options),
      [&](socket_t sock2, struct addrinfo& ai, bool& quit) -> bool {
        if (!intf.empty()) {
#ifdef USE_IF2IP
          auto ip_from_if = if2ip(address_family, intf);
          if (ip_from_if.empty()) { ip_from_if = intf; }
          if (!bind_ip_address(sock2, ip_from_if)) {
            error = Error::BindIPAddress;
            return false;
          }
#endif
        }

        set_nonblocking(sock2, true);

        auto ret =
          ::connect(sock2, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen));

        if (ret < 0) {
          if (is_connection_error()) {
            error = Error::Connection;
            return false;
          }
          error = wait_until_socket_is_ready(sock2, connection_timeout_sec,
            connection_timeout_usec);
          if (error != Error::Success) {
            if (error == Error::ConnectionTimeout) { quit = true; }
            return false;
          }
        }

        set_nonblocking(sock2, false);
        set_socket_opt_time(sock2, SOL_SOCKET, SO_RCVTIMEO, read_timeout_sec,
          read_timeout_usec);
        set_socket_opt_time(sock2, SOL_SOCKET, SO_SNDTIMEO, write_timeout_sec,
          write_timeout_usec);

        error = Error::Success;
        return true;
      });

    if (sock != INVALID_SOCKET) {
      error = Error::Success;
    }
    else {
      if (error == Error::Success) { error = Error::Connection; }
    }

    return sock;
  }

  socket_t create_client_socket(
    rsl::string_view host, rsl::string_view ip, int port,
    int address_family, bool tcp_nodelay, bool ipv6_v6only,
    SocketOptions socket_options, time_t connection_timeout_sec,
    time_t connection_timeout_usec, time_t read_timeout_sec,
    time_t read_timeout_usec, time_t write_timeout_sec,
    time_t write_timeout_usec, rsl::string_view intf, Error& error) {
    auto sock = create_socket(
      host, ip, port, address_family, 0, tcp_nodelay, ipv6_v6only,
      rsl::move(socket_options),
      [&](socket_t sock2, struct addrinfo& ai, bool& quit) -> bool {
        if (!intf.empty()) {
#ifdef USE_IF2IP
          auto ip_from_if = if2ip(address_family, intf);
          if (ip_from_if.empty()) { ip_from_if = intf; }
          if (!bind_ip_address(sock2, ip_from_if)) {
            error = Error::BindIPAddress;
            return false;
          }
#endif
        }

        set_nonblocking(sock2, true);

        auto ret =
          ::connect(sock2, ai.ai_addr, static_cast<socklen_t>(ai.ai_addrlen));

        if (ret < 0) {
          if (is_connection_error()) {
            error = Error::Connection;
            return false;
          }
          error = wait_until_socket_is_ready(sock2, connection_timeout_sec,
            connection_timeout_usec);
          if (error != Error::Success) {
            if (error == Error::ConnectionTimeout) { quit = true; }
            return false;
          }
        }

        set_nonblocking(sock2, false);
        set_socket_opt_time(sock2, SOL_SOCKET, SO_RCVTIMEO, read_timeout_sec,
          read_timeout_usec);
        set_socket_opt_time(sock2, SOL_SOCKET, SO_SNDTIMEO, write_timeout_sec,
          write_timeout_usec);

        error = Error::Success;
        return true;
      });

    if (sock != INVALID_SOCKET) {
      error = Error::Success;
    }
    else {
      if (error == Error::Success) { error = Error::Connection; }
    }

    return sock;
  }

  bool get_ip_and_port(const struct sockaddr_storage& addr,
    socklen_t addr_len, rsl::string& ip, int& port) {
    if (addr.ss_family == AF_INET) {
      port = ntohs(reinterpret_cast<const struct sockaddr_in*>(&addr)->sin_port);
    }
    else if (addr.ss_family == AF_INET6) {
      port =
        ntohs(reinterpret_cast<const struct sockaddr_in6*>(&addr)->sin6_port);
    }
    else {
      return false;
    }

    rsl::array<char, NI_MAXHOST> ipstr{};
    if (getnameinfo(reinterpret_cast<const struct sockaddr*>(&addr), addr_len,
      ipstr.data(), static_cast<socklen_t>(ipstr.size()), nullptr,
      0, NI_NUMERICHOST)) {
      return false;
    }

    ip.assign(ipstr.data());
    return true;
  }

  void get_local_ip_and_port(socket_t sock, rsl::string& ip, int& port) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    if (!getsockname(sock, reinterpret_cast<struct sockaddr*>(&addr),
      &addr_len)) {
      get_ip_and_port(addr, addr_len, ip, port);
    }
  }

  void get_remote_ip_and_port(socket_t sock, rsl::string& ip, int& port) {
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);

    if (!getpeername(sock, reinterpret_cast<struct sockaddr*>(&addr),
      &addr_len)) {
#ifndef _WIN32
      if (addr.ss_family == AF_UNIX) {
#if defined(__linux__)
        struct ucred ucred;
        socklen_t len = sizeof(ucred);
        if (getsockopt(sock, SOL_SOCKET, SO_PEERCRED, &ucred, &len) == 0) {
          port = ucred.pid;
        }
#elif defined(SOL_LOCAL) && defined(SO_PEERPID) // __APPLE__
        pid_t pid;
        socklen_t len = sizeof(pid);
        if (getsockopt(sock, SOL_LOCAL, SO_PEERPID, &pid, &len) == 0) {
          port = pid;
        }
#endif
        return;
      }
#endif
      get_ip_and_port(addr, addr_len, ip, port);
    }
  }

  constexpr unsigned int str2tag_core(const char* s, size_t l,
    unsigned int h) {
    return (l == 0)
      ? h
      : str2tag_core(
        s + 1, l - 1,
        // Unsets the 6 high bits of h, therefore no overflow happens
        (((rsl::numeric_limits<unsigned int>::max)() >> 6) &
          h * 33) ^
        static_cast<unsigned char>(*s));
  }

  unsigned int str2tag(rsl::string_view s) {
    return str2tag_core(s.data(), s.size(), 0);
  }

  inline int shutdown_socket(socket_t sock) {
#ifdef _WIN32
    return shutdown(sock, SD_BOTH);
#else
    return shutdown(sock, SHUT_RDWR);
#endif
  }

  rsl::string append_query_params(rsl::string_view path, const Params& params)
  {
    rsl::string path_with_query = path;
    thread_local const std::regex re("[^?]+\\?.*");
    auto delm = std::regex_match(path, re) ? '&' : '?';
    path_with_query += delm + detail::params_to_query_str(params);
    return path_with_query;
  }

  inline rsl::string params_to_query_str(const Params& params) {
    rsl::string query;

    for (auto it = params.begin(); it != params.end(); ++it) {
      if (it != params.begin()) { query += "&"; }
      query += it->first;
      query += "=";
      query += encode_query_param(it->second);
    }
    return query;
  }

  inline std::string make_multipart_data_boundary() {
    return "--cpp-httplib-multipart-data-" + detail::random_string(16);
  }

  inline std::string
    serialize_multipart_formdata_finish(const std::string& boundary) {
    return "--" + boundary + "--\r\n";
  }

  inline std::string
    serialize_multipart_formdata_get_content_type(const std::string& boundary) {
    return "multipart/form-data; boundary=" + boundary;
  }

  inline bool is_multipart_boundary_chars_valid(const std::string& boundary) {
    auto valid = true;
    for (size_t i = 0; i < boundary.size(); i++) {
      auto c = boundary[i];
      if (!std::isalnum(c) && c != '-' && c != '_') {
        valid = false;
        break;
      }
    }
    return valid;
  }

  rsl::string serialize_multipart_formdata(const MultipartFormDataItems& items, const rsl::string& boundary, bool finish) 
  {
    rsl::string body;

    for (const auto& item : items) {
      body += serialize_multipart_formdata_item_begin(item, boundary);
      body += item.content + serialize_multipart_formdata_item_end();
    }

    if (finish) { body += serialize_multipart_formdata_finish(boundary); }

    return body;
  }
}