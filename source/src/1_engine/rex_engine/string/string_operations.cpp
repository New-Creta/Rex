#include "rex_std_pch.h"

#include "string/string_operations.h"

namespace rex
{
    namespace string_operations
    {
        //-------------------------------------------------------------------------
        std::string& str_tolower(std::string& s)
        {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                           {
                               return gsl::narrow<uint8>(std::tolower(c));
                           });
            return s;
        }
        //-------------------------------------------------------------------------
        std::string& str_toupper(std::string& s)
        {
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c)
                           {
                               return gsl::narrow<uint8>(std::toupper(c));
                           });
            return s;
        }

        //-------------------------------------------------------------------------
        std::string str_tolower_copy(const std::string& s)
        {
            std::string new_s;
            new_s.reserve(s.size());
            for (auto& c : s)
            {
                new_s.push_back(gsl::narrow<uint8>(std::tolower(c)));
            }
            return new_s;
        }
        //-------------------------------------------------------------------------
        std::string str_toupper_copy(const std::string& s)
        {
            std::string new_s;
            new_s.reserve(s.size());
            for (auto& c : s)
            {
                new_s.push_back(gsl::narrow<uint8>(std::toupper(c)));
            }
            return s;
        }

#if __cplusplus >= 201703L
        //-------------------------------------------------------------------------
        bool starts_with(std::string_view str, std::string_view prefix)
        {
            return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
        }
        //-------------------------------------------------------------------------
        bool starts_with(const std::string& str, const char* prefix, unsigned prefixLen)
        {
            return str.size() >= prefixLen && 0 == str.compare(0, prefixLen, prefix, prefixLen);
        }
        //-------------------------------------------------------------------------
        bool starts_with(const std::string& str, const char* prefix)
        {
            return starts_with(str, prefix, std::string::traits_type::length(prefix));
        }

        //-------------------------------------------------------------------------
        bool ends_with(std::string_view str, std::string_view suffix)
        {
            return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
        }
        //-------------------------------------------------------------------------
        bool ends_with(const std::string& str, const char* suffix, unsigned suffixLen)
        {
            return str.size() >= suffixLen && 0 == str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen);
        }
        //-------------------------------------------------------------------------
        bool ends_with(const std::string& str, const char* suffix)
        {
            return ends_with(str, suffix, std::string::traits_type::length(suffix));
        }
#else
        //-------------------------------------------------------------------------
        bool starts_with(const std::string& str, const std::string& prefix)
        {
            return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
        }
        //-------------------------------------------------------------------------
        bool starts_with(const std::string& str, const char* prefix, unsigned prefixLen)
        {
            return str.size() >= prefixLen && 0 == str.compare(0, prefixLen, prefix, prefixLen);
        }
        //-------------------------------------------------------------------------
        bool starts_with(const std::string& str, const char* prefix)
        {
            return starts_with(str, prefix, gsl::narrow<uint32>(std::string::traits_type::length(prefix)));
        }
        //-------------------------------------------------------------------------
        bool ends_with(const std::string& str, const std::string& suffix)
        {
            return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
        }
        //-------------------------------------------------------------------------
        bool ends_with(const std::string& str, const char* suffix, unsigned suffixLen)
        {
            return str.size() >= suffixLen && 0 == str.compare(str.size() - suffixLen, suffixLen, suffix, suffixLen);
        }
        //-------------------------------------------------------------------------
        bool ends_with(const std::string& str, const char* suffix)
        {
            return ends_with(str, suffix, gsl::narrow<uint32>(std::string::traits_type::length(suffix)));
        }

#endif

        //-------------------------------------------------------------------------
        std::vector<std::string> split(const std::string& in, const std::string& delimiters)
        {
            std::size_t prev = 0;
            std::size_t currentPos = 0;
            std::vector<std::string> results;

            while ((currentPos = in.find_first_of(delimiters, prev)) != std::string::npos)
            {
                if (currentPos > prev)
                {
                    results.push_back(in.substr(prev, currentPos - prev));
                }
                prev = currentPos + 1;
            }

            if (prev < in.length())
            {
                results.push_back(in.substr(prev));
            }
            return results;
        }

        //-------------------------------------------------------------------------
        std::string replace_all(std::string str, const std::string& from, const std::string& to)
        {
            int64 start_pos = 0;
            while ((start_pos = str.find(from, start_pos)) != std::string::npos)
            {
                str.replace(start_pos, from.length(), to);
                start_pos += to.length();  // Handles case where 'to' is a substring of 'from'
            }
            return str;
        }

        //-------------------------------------------------------------------------
        void ltrim(std::string& s)
        {
            s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch)
                                            {
                                                return !std::isspace(ch);
                                            }));
        }
        //-------------------------------------------------------------------------
        void rtrim(std::string& s)
        {
            s.erase(std::find_if(s.rbegin(), s.rend(),
                                 [](unsigned char ch)
                                 {
                                     return !std::isspace(ch);
                                 })
                        .base(),
                    s.end());
        }
        //-------------------------------------------------------------------------
        void trim(std::string& s)
        {
            ltrim(s);
            rtrim(s);
        }
        //-------------------------------------------------------------------------
        std::string ltrim_copy(std::string s)
        {
            ltrim(s);
            return s;
        }
        //-------------------------------------------------------------------------
        std::string rtrim_copy(std::string s)
        {
            rtrim(s);
            return s;
        }
        //-------------------------------------------------------------------------
        std::string trim_copy(std::string s)
        {
            trim(s);
            return s;
        }
    }
}