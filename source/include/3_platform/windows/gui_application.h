#pragma once

#include "core_application.h"

namespace rex
{
    struct FrameInfo;

    namespace win32
    {
        class GuiApplication : public CoreApplication
        {
        public:
            GuiApplication(const ApplicationDescription& description);
            ~GuiApplication() override;

        protected:
            void platform_initialize() override;
            void platform_update(const FrameInfo& info) override;
            void platform_shutdown() override;

            virtual void app_initialize() = 0;
            virtual void app_update(const FrameInfo& info) = 0;
            virtual void app_shutdown() = 0;
        };
    }
}