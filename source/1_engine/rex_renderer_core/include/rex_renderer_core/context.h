#pragma once

namespace rex
{
    namespace context
    {
        bool create(void* userData);

        namespace os
        {
            // OS specific implementation
            bool pre_setup(void* userData);
            bool post_setup(void* userData);

            void rollback(void* userData);
        }

        namespace backend
        {
            // Graphics API specific implementation
            bool create(void* userData);
        }
    }
}