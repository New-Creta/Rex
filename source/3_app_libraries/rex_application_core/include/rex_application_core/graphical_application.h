#pragma once

#include "rex_windows/platform.h"
#include "rex_windows/gui_application.h"

namespace rex
{
    struct FrameInfo;

    class GraphicalApplication : public GuiApplication
    {
    public:
        GraphicalApplication(const ApplicationDescription& description);
        ~GraphicalApplication() override;

    protected:
        void app_initialize() override;
        void app_update(const FrameInfo& info) override;
        void app_shutdown() override;

        virtual void initialize() {};
        virtual void update(const FrameInfo& /*info*/) {};
        virtual void shutdown() {};
    };
}