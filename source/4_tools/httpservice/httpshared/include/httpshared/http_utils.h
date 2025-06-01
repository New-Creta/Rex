#pragma once

#include "rex_std/string.h"
#include "rex_std/string_view.h"

namespace http
{
  rsl::string escape_abstract_namespace_unix_domain(rsl::string_view s);
  socket_t create_client_socket(
    rsl::string_view host, rsl::string_view ip, int port,
    int address_family, bool tcp_nodelay, bool ipv6_v6only,
    SocketOptions socket_options, time_t connection_timeout_sec,
    time_t connection_timeout_usec, time_t read_timeout_sec,
    time_t read_timeout_usec, time_t write_timeout_sec,
    time_t write_timeout_usec, rsl::string_view intf, Error& error);

  rsl::string append_query_params(rsl::string_view path, const Params& params);

  rsl::string params_to_query_str(const Params& params);
  rsl::string make_multipart_data_boundary();

  rsl::string serialize_multipart_formdata_finish(const rsl::string& boundary);
  rsl::string serialize_multipart_formdata_get_content_type(const rsl::string& boundary);
  bool is_multipart_boundary_chars_valid(const rsl::string& boundary);
  rsl::string serialize_multipart_formdata(const MultipartFormDataItems& items, const rsl::string& boundary, bool finish = true);
}