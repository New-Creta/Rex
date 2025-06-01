#pragma once

#include "rex_std/string.h"
#include "rex_std/unordered_map.h"
#include "rex_std/chrono.h"

#include "httpshared/http_types.h"

namespace http
{
  struct Request {
    HttpMethod method;
    rsl::string path;
    Headers headers;
    rsl::string body;

    rsl::string remote_addr;
    int remote_port = -1;
    rsl::string local_addr;
    int local_port = -1;

    // for server
    rsl::string version;
    rsl::string target;
    Params params;
    MultipartFormDataMap files;
    Ranges ranges;
    Match matches;
    rsl::unordered_map<rsl::string, rsl::string> path_params;

    // for client
    ResponseHandler response_handler;
    ContentReceiverWithProgress content_receiver;
    Progress progress;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    const SSL* ssl = nullptr;
#endif

    bool has_header(const rsl::string& key) const;
    rsl::string get_header_value(const rsl::string& key, const char* def = "",
      size_t id = 0) const;
    uint64_t get_header_value_u64(const rsl::string& key, uint64_t def = 0,
      size_t id = 0) const;
    size_t get_header_value_count(const rsl::string& key) const;
    void set_header(const rsl::string& key, const rsl::string& val);

    bool has_param(const rsl::string& key) const;
    rsl::string get_param_value(const rsl::string& key, size_t id = 0) const;
    size_t get_param_value_count(const rsl::string& key) const;

    bool is_multipart_form_data() const;

    bool has_file(const rsl::string& key) const;
    MultipartFormData get_file_value(const rsl::string& key) const;
    rsl::vector<MultipartFormData> get_file_values(const rsl::string& key) const;

    // private members...
    size_t redirect_count = CPPHTTPLIB_REDIRECT_MAX_COUNT;
    size_t content_length = 0;
    ContentProvider content_provider;
    bool is_chunked_content_provider = false;
    size_t authorization_count = 0;
    rsl::chrono::time_point<rsl::chrono::steady_clock> start_time =
      (rsl::chrono::steady_clock::time_point::min)();
  };

}