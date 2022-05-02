#pragma once

#include <memory>

namespace rex
{
    class ApplicationArguments;

    struct ApplicationDescription
    {
        ApplicationDescription()
            :max_fps(144)
            ,window_width(1280)
            ,window_height(720)
        {}

        int32 max_fps;

        int32 window_width;
        int32 window_height;
    };

    class CoreApplication
    {
    public:
        static CoreApplication* get_instance();

        CoreApplication(const ApplicationDescription& description);
        virtual ~CoreApplication();

        int32 run();

        virtual void quit() = 0;

    protected:
        const ApplicationDescription& get_application_description() const;

        bool initialize();
        bool shutdown();
        
        virtual bool platform_initialize() = 0;
        virtual bool platform_shutdown() = 0;

    private:
        static CoreApplication* s_instance;

        ApplicationDescription m_description;
    };

    // This will be implemented by the CLIENT!
    CoreApplication* create_application();
}