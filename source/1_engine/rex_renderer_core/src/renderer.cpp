#include "rex_renderer_core/renderer.h"
#include "rex_renderer_core/commands/render_cmd.h"
#include "rex_renderer_core/slot_resource.h"
#include "rex_engine/defines.h"
#include "rex_engine/ring_buffer.h""

#if REX_SINGLE_THREADED
#define add_cmd(cmd) exec_cmd(cmd)
#else
#define add_cmd(cmd) g_ctx.cmd_buffer.put(cmd)
#endif

namespace rex
{
    namespace renderer
    {
        struct Context
        {
            SlotResources slot_resources;

            RingBuffer<RenderCommand> cmd_buffer;
        };

        Context g_ctx;

        //-------------------------------------------------------------------------
        bool initialize(const OutputWindowUserData& userData, u32 maxCommands)
        {
            UNUSED_PARAM(userData);
            UNUSED_PARAM(maxCommands);

            return backend::initialize(userData);
        }
        //-------------------------------------------------------------------------
        void shutdown()
        {
            backend::shutdown();
        }

        //-------------------------------------------------------------------------
        u32 create_clear_state(const parameters::ClearState& clearStateParams)
        {
            RenderCommand cmd;

            u32 resource_slot = slot_resources_get_next(&g_ctx.slot_resources);

            cmd.command_type = CommandType::CREATE_CLEAR_STATE;
            cmd.clear_state_params = clearStateParams;
            cmd.resource_slot = resource_slot;

            add_cmd(cmd);

            return resource_slot;
        }

        //-------------------------------------------------------------------------
        void clear(u32 clearStateSlot)
        {
            RenderCommand cmd;

            cmd.command_type = CommandType::CLEAR;
            cmd.clear.clear_state = clearStateSlot;

            add_cmd(cmd);
        }

    } // namespace renderer
} // namespace rex