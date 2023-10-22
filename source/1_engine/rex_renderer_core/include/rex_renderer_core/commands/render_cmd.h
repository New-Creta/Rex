#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/commands/clear_cmd.h"

#include "rex_renderer_core/parameters/clear_state_params.h"

namespace rex
{
    namespace renderer
    {
        enum class CommandType
        {
            NONE = 0,
            NEW_FRAME,
            CLEAR,
            CLEAR_TEXTURE,
            PRESENT,
            LOAD_SHADER,
            SET_SHADER,
            LINK_SHADER,
            CREATE_INPUT_LAYOUT,
            SET_INPUT_LAYOUT,
            CREATE_BUFFER,
            SET_VERTEX_BUFFER,
            SET_INDEX_BUFFER,
            DRAW,
            DRAW_INDEXED,
            DRAW_INDEXED_INSTANCED,
            CREATE_TEXTURE,
            RELEASE_SHADER,
            RELEASE_BUFFER,
            RELEASE_TEXTURE_2D,
            CREATE_SAMPLER,
            SET_TEXTURE,
            CREATE_RASTER_STATE,
            SET_RASTER_STATE,
            SET_VIEWPORT,
            SET_SCISSOR_RECT,
            SET_VIEWPORT_RATIO,
            SET_SCISSOR_RECT_RATIO,
            RELEASE_RASTER_STATE,
            CREATE_BLEND_STATE,
            SET_BLEND_STATE,
            SET_CONSTANT_BUFFER,
            SET_STRUCTURED_BUFFER,
            UPDATE_BUFFER,
            CREATE_DEPTH_STENCIL_STATE,
            SET_DEPTH_STENCIL_STATE,
            UPDATE_QUERIES,
            CREATE_RENDER_TARGET,
            SET_TARGETS,
            RELEASE_BLEND_STATE,
            RELEASE_RENDER_TARGET,
            RELEASE_INPUT_LAYOUT,
            RELEASE_SAMPLER,
            RELEASE_PROGRAM,
            RELEASE_CLEAR_STATE,
            RELEASE_DEPTH_STENCIL_STATE,
            CREATE_SO_SHADER,
            SET_SO_TARGET,
            RESOLVE_TARGET,
            DRAW_AUTO,
            MAP_RESOURCE,
            REPLACE_RESOURCE,
            CREATE_CLEAR_STATE,
            PUSH_PERF_MARKER,
            POP_PERF_MARKER,
            DISPATCH_COMPUTE,
            SET_STENCIL_REF
        };

        struct RenderCommand
        {
            CommandType command_type;
            u32 resource_slot;
            u64 frame_index;

            union
            {
                commands::Clear                 clear;
                parameters::ClearState          clear_state_params;
            };
        };
    }
}