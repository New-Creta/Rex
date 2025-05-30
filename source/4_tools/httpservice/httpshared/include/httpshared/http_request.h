#pragma once

namespace http
{
  struct Request {
    std::string method;
    std::string path;
    Headers headers;
    std::string body;

    std::string remote_addr;
    int remote_port = -1;
    std::string local_addr;
    int local_port = -1;

    // for server
    std::string version;
    std::string target;
    Params params;
    MultipartFormDataMap files;
    Ranges ranges;
    Match matches;
    std::unordered_map<std::string, std::string> path_params;

    // for client
    ResponseHandler response_handler;
    ContentReceiverWithProgress content_receiver;
    Progress progress;
#ifdef CPPHTTPLIB_OPENSSL_SUPPORT
    const SSL* ssl = nullptr;
#endif

    bool has_header(const std::string& key) const;
    std::string get_header_value(const std::string& key, const char* def = "",
      size_t id = 0) const;
    uint64_t get_header_value_u64(const std::string& key, uint64_t def = 0,
      size_t id = 0) const;
    size_t get_header_value_count(const std::string& key) const;
    void set_header(const std::string& key, const std::string& val);

    bool has_param(const std::string& key) const;
    std::string get_param_value(const std::string& key, size_t id = 0) const;
    size_t get_param_value_count(const std::string& key) const;

    bool is_multipart_form_data() const;

    bool has_file(const std::string& key) const;
    MultipartFormData get_file_value(const std::string& key) const;
    std::vector<MultipartFormData> get_file_values(const std::string& key) const;

    // private members...
    size_t redirect_count_ = CPPHTTPLIB_REDIRECT_MAX_COUNT;
    size_t content_length_ = 0;
    ContentProvider content_provider_;
    bool is_chunked_content_provider_ = false;
    size_t authorization_count_ = 0;
  };

}