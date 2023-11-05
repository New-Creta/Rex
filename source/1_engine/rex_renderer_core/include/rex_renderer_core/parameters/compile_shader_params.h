#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/shader_type.h"

#include <rex_std/string.h>
#include <rex_std/bonus/string/stack_string.h>

namespace rex
{
    namespace renderer
    {
        namespace parameters
        {
            rsl::tiny_stack_string s_vertex_entry_point = "VS";
            rsl::tiny_stack_string s_pixel_entry_point = "PS";
            rsl::tiny_stack_string s_geometry_entry_point = "GS";
            rsl::tiny_stack_string s_compute_entry_point = "CS";

            struct CompileShader
            {
                CompileShader()
                    :shader_type(ShaderType::NONE)
                    ,shader_name("")
                    ,shader_entry_point("")
                    ,shader_code(nullptr)
                    ,shader_code_size(0)
                    ,shader_constant_buffer_count(0)
                {
                    switch (shader_type)
                    {
                    case ShaderType::VERTEX: shader_entry_point = s_vertex_entry_point.data(); break;
                    case ShaderType::PIXEL: shader_entry_point = s_pixel_entry_point.data(); break;
                    case ShaderType::GEOMETRY: shader_entry_point = s_geometry_entry_point.data(); break;
                    case ShaderType::COMPUTE: shader_entry_point = s_compute_entry_point.data(); break;
                    }
                }

                ShaderType shader_type;
                
                rsl::small_stack_string shader_name;
                
                char8* shader_entry_point;
                char8* shader_feature_target;

                char8* shader_code;
                u64 shader_code_size;

                u32 shader_constant_buffer_count;
            };
        }
    }
}