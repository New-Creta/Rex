#pragma once

#include "rex_engine/types.h"

#include "rex_renderer_core/commands/clear_cmd.h"
#include "rex_renderer_core/commands/release_resource_cmd.h"
#include "rex_renderer_core/commands/draw_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_cmd.h"
#include "rex_renderer_core/commands/draw_indexed_instanced_cmd.h"
#include "rex_renderer_core/commands/draw_instanced_cmd.h"
#include "rex_renderer_core/commands/set_render_target_cmd.h"
#include "rex_renderer_core/commands/set_input_layout_cmd.h"
#include "rex_renderer_core/commands/set_vertex_buffer_cmd.h"
#include "rex_renderer_core/commands/set_index_buffer_cmd.h"
#include "rex_renderer_core/commands/set_shader_cmd.h"
#include "rex_renderer_core/commands/set_raster_state_cmd.h"
#include "rex_renderer_core/commands/set_pipeline_state_cmd.h"
#include "rex_renderer_core/commands/begin_draw_cmd.h"
#include "rex_renderer_core/commands/end_draw_cmd.h"
#include "rex_renderer_core/commands/set_constant_buffer_cmd.h"

#include "rex_renderer_core/parameters/clear_state_params.h"
#include "rex_renderer_core/parameters/raster_state_params.h"
#include "rex_renderer_core/parameters/create_input_layout_params.h"
#include "rex_renderer_core/parameters/create_buffer_params.h"
#include "rex_renderer_core/parameters/create_constant_buffer_params.h"
#include "rex_renderer_core/parameters/load_shader_params.h"
#include "rex_renderer_core/parameters/link_shader_params.h"
#include "rex_renderer_core/parameters/create_pipeline_state_params.h"
#include "rex_renderer_core/parameters/update_constant_buffer_params.h"
 
#include "rex_renderer_core/viewport.h"
#include "rex_renderer_core/scissor_rect.h"

namespace rex
{
    namespace renderer
    {
        enum class CommandType
        {
            NONE = 0,
            CREATE_CLEAR_STATE,
            CREATE_RASTER_STATE,
            CREATE_INPUT_LAYOUT_STATE,
            CREATE_VERTEX_BUFFER,
            CREATE_INDEX_BUFFER,
            CREATE_CONSTANT_BUFFER,
            CREATE_PIPELINE_STATE,
            CREATE_FRAME_RESOURCE,
            LOAD_SHADER,
            LINK_SHADER,
            COMPILE_SHADER,
            RELEASE_RESOURCE,
            UPDATE_CONSTANT_BUFFER,
            CLEAR,
            BEGIN_DRAW,
            END_DRAW,
            DRAW,
            DRAW_INDEXED,
            DRAW_INDEXED_INSTANCED,
            DRAW_INSTANCED,
            SET_RENDER_TARGETS,
            SET_INPUT_LAYOUT,
            SET_VIEWPORT,
            SET_SCISSOR_RECT,
            SET_VERTEX_BUFFER,
            SET_INDEX_BUFFER,
            SET_SHADER,
            SET_RASTER_STATE,
            SET_PIPELINE_STATE,
            SET_CONSTANT_BUFFER,
            NEW_FRAME,
            END_FRAME,
            PRESENT
        };

        struct RenderCommand
        {
            RenderCommand()
                :command_type(CommandType::NONE)
                ,resource_slot(REX_INVALID_INDEX)
                ,frame_index(REX_INVALID_INDEX)
            {}

            CommandType command_type;

            s32 resource_slot;
            s64 frame_index;

            union
            {
                commands::Clear                     clear;
                commands::ReleaseResource           release_resource;
                commands::BeginDraw                 begin_draw;
                commands::EndDraw                   end_draw;
                commands::Draw                      draw;
                commands::DrawIndexed               draw_indexed;
                commands::DrawIndexedInstanced      draw_indexed_instanced;
                commands::DrawInstanced             draw_instanced;
                commands::SetRenderTarget           set_render_target;
                commands::SetInputLayout            set_input_layout;
                commands::SetVertexBuffer           set_vertex_buffer;
                commands::SetIndexBuffer            set_index_buffer;
                commands::SetShader                 set_shader;
                commands::SetRasterState            set_raster_state;
                commands::SetPipelineState          set_pipeline_state;
                commands::SetConstantBuffer         set_constant_buffer;

                parameters::ClearState              clear_state_params;
                parameters::RasterState             raster_state_params;
                parameters::CreateInputLayout       create_input_layout_params;
                parameters::CreateBuffer            create_buffer_params;
                parameters::CreateConstantBuffer    create_constant_buffer_params;
                parameters::LoadShader              load_shader_params;
                parameters::LinkShader              link_shader_params;
                parameters::CompileShader           compile_shader_params;
                parameters::CreatePipelineState     create_pipeline_state_params;
                parameters::UpdateConstantBuffer    update_constant_buffer_params;

                Viewport                            viewport;
                ScissorRect                         scissor_rect;
            };
        };
    }
}