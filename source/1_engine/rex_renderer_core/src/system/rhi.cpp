#include "rex_renderer_core/system/rhi.h"

namespace rex
{
  namespace rhi
  {
    CommandsFrame::CommandsFrame()
    {
      reset_command_list(ResourceSlot::make_invalid());
    }
    CommandsFrame::~CommandsFrame()
    {
      exec_command_list();
      flush_command_queue();
    }
  }
}