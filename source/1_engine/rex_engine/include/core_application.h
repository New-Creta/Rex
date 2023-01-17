#pragma once

#include <memory>

namespace rex
{
    struct FrameInfo;

    struct ApplicationDescription
    {
        ApplicationDescription()
            :max_fps(144)
        {}

        int max_fps;
    };

    class CoreApplication
    {
    public:
        CoreApplication(const ApplicationDescription& description);
        virtual ~CoreApplication();

        bool is_running() const;

        int run();
        void quit();

    protected:
        void mark_for_destroy();

        virtual void platform_initialize() = 0;
        virtual void platform_update(const FrameInfo& info) = 0;
        virtual void platform_shutdown() = 0;

    private:
        struct Internal;
        std::unique_ptr<Internal> m_internal_ptr;
    };

    // This will be implemented by the CLIENT!
    CoreApplication* create_application();
}