#include "rex_renderer/renderer.h"
#include "rex_engine/defines.h"

namespace rex
{
    namespace renderer
    {

        //-------------------------------------------------------------------------
        bool initialize(void* userData, u32 maxCommands)
        {
          UNUSED_PARAM(userData);
          UNUSED_PARAM(maxCommands);

          return true;
        }

    }
}