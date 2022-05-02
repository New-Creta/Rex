#pragma once

namespace rex
{
    namespace string_operations
    {
        std::string& str_tolower(std::string& s);
        std::string& str_toupper(std::string& s);
        std::string str_tolower_copy(const std::string& s);
        std::string str_toupper_copy(const std::string& s);

#if __cplusplus >= 201703L

        bool starts_with(std::string_view str, std::string_view prefix);
        bool starts_with(const std::string& str, const char* prefix, unsigned prefixLen);
        bool starts_with(const std::string& str, const char* prefix);
        bool ends_with(std::string_view str, std::string_view suffix);
        bool ends_with(const std::string& str, const char* suffix, unsigned suffixLen);
        bool ends_with(const std::string& str, const char* suffix);

#else

        bool starts_with(const std::string& str, const std::string& prefix);
        bool starts_with(const std::string& str, const char* prefix, unsigned prefixLen);
        bool starts_with(const std::string& str, const char* prefix);
        bool ends_with(const std::string& str, const std::string& suffix);
        bool ends_with(const std::string& str, const char* suffix, unsigned suffixLen);
        bool ends_with(const std::string& str, const char* suffix);

#endif

        std::vector<std::string> split(const std::string& in, const std::string& delimiters);

        std::string replace_all(std::string str, const std::string& from, const std::string& to);

        void ltrim(std::string& s);
        void rtrim(std::string& s);
        void trim(std::string& s);

        std::string ltrim_copy(std::string s);
        std::string rtrim_copy(std::string s);
        std::string trim_copy(std::string s);
    }
}