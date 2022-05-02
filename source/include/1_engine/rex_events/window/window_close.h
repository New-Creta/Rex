#pragma once

#include "window/window_event.h"

namespace rex
{
    namespace events
    {
        class WindowClose : public WindowEvent
        {
        public:
            EVENT_CLASS_TYPE(WindowClose);

            WindowClose(uint32 windowID/* , const memory::Blob& nativeEvent */);
            ~WindowClose() override;

            bool can_consume() const override;

            std::string to_string() const override;
        };
    }
}
