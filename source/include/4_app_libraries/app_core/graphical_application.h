#pragma once

#include "platform.h"
#include "gui_application.h"

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

        virtual initialize() {};
        virtual update(const FrameInfo& /*info*/) {};
        virtual shutdown() {};
    }
}