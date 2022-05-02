#include "rex_engine_pch.h"

#include "application_arguments_parser.h"

#include "string/string_operations.h"

namespace rex
{
    namespace application_argument_parser
    {
        //-------------------------------------------------------------------------
        ApplicationArgument parse(const std::string& input)
        {
            auto it = input.find('=');
            if (it == std::string::npos)
            {
                return {};
            }

            // Split on '='
            auto result = string_operations::split(input, "=");

            // // There can only be 2 values
            // R_ASSERT(result.size() == 2);

            // Trim any spaces
            string_operations::trim(result[1]);

            ApplicationArgument argument;

            argument.key = result[0];
            argument.value = result[1];

            return argument;
        }
    }
}