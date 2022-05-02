#pragma once

#include "platform.h"
#include "win32_application.h"

namespace rex
{
    struct FrameInfo;

    class GraphicalApplication : public win32::Application
    {
    public:
        GraphicalApplication(const ApplicationDescription& description);
        ~GraphicalApplication() override;

    protected:
        void on_app_initialize() override;
        void on_app_update(const FrameInfo& info) override;
        void on_app_shutdown() override;

        virtual void initialize() {};
        virtual void update(const FrameInfo& /*info*/) {};
        virtual void shutdown() {};
    };
}