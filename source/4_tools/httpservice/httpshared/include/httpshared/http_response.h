#pragma once

namespace http
{
  struct Response {
    std::string version;
    int status = -1;
    std::string reason;
    Headers headers;
    std::string body;
    std::string location; // Redirect location

    bool has_header(const std::string& key) const;
    std::string get_header_value(const std::string& key, const char* def = "",
      size_t id = 0) const;
    uint64_t get_header_value_u64(const std::string& key, uint64_t def = 0,
      size_t id = 0) const;
    size_t get_header_value_count(const std::string& key) const;
    void set_header(const std::string& key, const std::string& val);

    void set_redirect(const std::string& url, int status = StatusCode::Found_302);
    void set_content(const char* s, size_t n, const std::string& content_type);
    void set_content(const std::string& s, const std::string& content_type);
    void set_content(std::string&& s, const std::string& content_type);

    void set_content_provider(
      size_t length, const std::string& content_type, ContentProvider provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

    void set_content_provider(
      const std::string& content_type, ContentProviderWithoutLength provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

    void set_chunked_content_provider(
      const std::string& content_type, ContentProviderWithoutLength provider,
      ContentProviderResourceReleaser resource_releaser = nullptr);

    void set_file_content(const std::string& path,
      const std::string& content_type);
    void set_file_content(const std::string& path);

    Response() = default;
    Response(const Response&) = default;
    Response& operator=(const Response&) = default;
    Response(Response&&) = default;
    Response& operator=(Response&&) = default;
    ~Response() {
      if (content_provider_resource_releaser_) {
        content_provider_resource_releaser_(content_provider_success_);
      }
    }

    // private members...
    size_t content_length_ = 0;
    ContentProvider content_provider_;
    ContentProviderResourceReleaser content_provider_resource_releaser_;
    bool is_chunked_content_provider_ = false;
    bool content_provider_success_ = false;
    std::string file_content_path_;
    std::string file_content_content_type_;
  };

}