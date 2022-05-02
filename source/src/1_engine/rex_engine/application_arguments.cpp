#include "rex_engine_pch.h"

#include "application_arguments.h"
#include "application_arguments_parser.h"

#include "string/string_operations.h"

namespace rex
{
    //-------------------------------------------------------------------------
    ApplicationArguments::ApplicationArguments(int32 inArgc, char** inArgv)
        : m_argc(inArgc)
        , m_argv(inArgv)
    {
        m_arguments.reserve(inArgc);

        // // First argument is our working directory
        // R_TODO("Capture the working directory when parsing application arguments");
        
        // R_INFO("[APPLICATION ARGUMENT] idx: 0, value: {0}", inArgv[0]);

        for (int i = 1; i < inArgc; ++i)
        {
            // R_INFO("[APPLICATION ARGUMENT] idx: {0}, value: {1}", i, inArgv[i]);

            ApplicationArgument argument = application_argument_parser::parse(inArgv[i]);

            if (argument.key.empty() || argument.value.empty())
            {
                continue;
            }

            m_arguments.insert(std::make_pair(argument.key, argument.value));
        }
    }
    //-------------------------------------------------------------------------
    ApplicationArguments::~ApplicationArguments() = default;

    //-------------------------------------------------------------------------
    bool ApplicationArguments::has_arguments() const
    {
        return !m_arguments.empty();
    }
    //-------------------------------------------------------------------------
    bool ApplicationArguments::has_argument(const std::string& arg) const
    {
        return m_arguments.find(arg) != std::cend(m_arguments);
    }

    //-------------------------------------------------------------------------
    std::string ApplicationArguments::get_argument_value(const std::string& arg) const
    {
        auto it = m_arguments.find(arg);
        if (it == std::cend(m_arguments))
        {
            return {};
        }

        return it->second;
    }

    //-------------------------------------------------------------------------
    int32 ApplicationArguments::get_argument_count() const
    {
        return m_argc;
    }
    //-------------------------------------------------------------------------
    char** ApplicationArguments::get_argument_values() const
    {
        return m_argv;
    }
}