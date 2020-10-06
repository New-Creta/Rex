#pragma once

namespace rex
{
    namespace graphics
    {
        enum class ShaderDataType
        {
            NONE = 0,
            FLOAT,
            FLOAT2,
            FLOAT3,
            FLOAT4,

            MAT3,
            MAT4,

            INT,
            INT2,
            INT3,
            INT4,

            BOOL
        };

        //-------------------------------------------------------------------------
        static unsigned int shaderDataTypeSize(ShaderDataType type)
        {
            switch (type)
            {
            case ShaderDataType::FLOAT:    return 4;
            case ShaderDataType::FLOAT2:   return 4 * 2;
            case ShaderDataType::FLOAT3:   return 4 * 3;
            case ShaderDataType::FLOAT4:   return 4 * 4;
            case ShaderDataType::MAT3:     return 4 * 3 * 3;
            case ShaderDataType::MAT4:     return 4 * 4 * 4;
            case ShaderDataType::INT:      return 4;
            case ShaderDataType::INT2:     return 4 * 2;
            case ShaderDataType::INT3:     return 4 * 3;
            case ShaderDataType::INT4:     return 4 * 4;
            case ShaderDataType::BOOL:     return 1;
            }

            RX_ASSERT_X(false, "Unknown ShaderDataType!");
            return 0;
        }
    }
}