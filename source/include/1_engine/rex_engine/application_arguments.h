#pragma once

namespace rex
{
    class ApplicationArguments
    {
    public:
        ApplicationArguments(int32 argc, char** argv);
        ~ApplicationArguments();

        bool has_arguments() const;
        bool has_argument(const std::string& arg) const;

        std::string get_argument_value(const std::string& arg) const;

        int32 get_argument_count() const;
        char** get_argument_values() const;

    private:
        using Arguments = std::unordered_map<std::string, std::string>;

        Arguments m_arguments;

        int32 m_argc;
        char** m_argv;
    };
}