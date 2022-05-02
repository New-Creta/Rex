#pragma once

namespace rex
{
    struct ApplicationArgument
    {
        std::string key;
        std::string value;
    };

    namespace application_argument_parser
    {
        ApplicationArgument parse(const std::string& input);
    }
}