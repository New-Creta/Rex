#pragma once

#include "rex_engine/types.h"
#include "rex_engine/defines.h"

#include "rex_engine/state_controller.h"

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            enum class UsageFlags
            {
                USAGE_DEFAULT = BIT(0),     // gpu read and write, d3d can updatesubresource with usage default
                USAGE_IMMUTABLE = BIT(1),   // gpu read only
                USAGE_DYNAMIC = BIT(2),     // dynamic
                USAGE_STAGING = BIT(3),     // cpu access
            };

            enum class BindFlags
            {
                BIND_SHADER_RESOURCE = BIT(0),
                BIND_VERTEX_BUFFER = BIT(1),
                BIND_INDEX_BUFFER = BIT(2),
                BIND_CONSTANT_BUFFER = BIT(3),
                BIND_RENDER_TARGET = BIT(4),
                BIND_DEPTH_STENCIL = BIT(4),
                BIND_SHADER_WRITE = BIT(6),
                STREAM_OUT_VERTEX_BUFFER = BIT(7)
            };

            enum class CPUAccessFlags
            {
                CPU_ACCESS_WRITE = BIT(0),
                CPU_ACCESS_READ = BIT(1)
            };

            struct CreateBuffer
            {
                CreateBuffer()
                    :usage_flags()
                    ,bind_flags()
                    ,cpu_access_flags()
                    ,buffer_size(0)
                    ,stride(0)
                    ,data(nullptr)
                {}

                StateController<UsageFlags> usage_flags;
                StateController<BindFlags> bind_flags;
                StateController<CPUAccessFlags> cpu_access_flags;

                u32 buffer_size;
                u32 stride; // for structured buffer

                void* data;
            };
        }
    }
}