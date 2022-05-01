#include "rex_std_pch.h"

#include "string/string_conversions.h"

namespace rex
{
    namespace string_conversion
    {
        //-------------------------------------------------------------------------
        int32 string_to_int(const std::string& input)
        {
            int32 result{};

            // Check, if there is any letter in the input string
            if (std::any_of(input.begin(), input.end(), isalpha))
            {
                R_ERROR("Input: {0}, is not a number", input);
                return -1;
            }

            // Check, if string has more than 10 characters
            if (input.length() > 10)
            {
                R_ERROR("Buffer overflow");
                return -1;
            }

            result = std::stoi(input);

            return result;
        }
    }
}