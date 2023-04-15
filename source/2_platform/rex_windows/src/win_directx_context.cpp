#include "rex_engine/types.h"
#include "rex_renderer_core/context.h"
#include "rex_windows/log.h"

#include <Windows.h>

namespace rex
{
    namespace context
    {
        namespace os
        {
            // OS specific implementation
            bool pre_setup(void* /*userData*/)
            {
                return true;
            }
            bool post_setup(void* /*userData*/)
            {
                return true;
            }

            void rollback(void* /*userData*/)
            {
                
            }
        } // namespace os
    }   // namespace context
} // namespace rex
