#include "engine_pch.h"

#include "core_application.h"

#include "world.h"

#include "frameinfo/frameinfo.h"

#include <functional>
#include <thread>
#include <chrono>

#include <assert.h>

namespace rex
{
    World g_world;

    struct CoreApplication::Internal
    {
        //-------------------------------------------------------------------------
        Internal(const ApplicationDescription& description)
            :is_running(false)
            ,is_marked_for_destroy(false)
            ,app_description(description)
        {
        }

        //-------------------------------------------------------------------------
        void initialize()
        {
            on_initialize();
        }
        //-------------------------------------------------------------------------
        void update()
        {
            is_running = true;

            while (is_running)
            {
                loop();

                is_running = !is_marked_for_destroy;
            }
        }
        //-------------------------------------------------------------------------
        void shutdown()
        {
            on_shutdown();
        }

        //-------------------------------------------------------------------------
        void loop()
        {
            FrameInfo info = { World::getDeltaTime(), World::getFramesPerSecond() };

            on_update(info);

            g_world.update();

            //
            // Cap framerate to "max_fps".
            // Safe resources of the machine we are running on.
            //
            std::chrono::milliseconds actual_time(static_cast<int>(std::lrint(1000.0f / g_world.getFramesPerSecond().get())));
            std::chrono::milliseconds desired_time(static_cast<int>(std::lrint(1000.0f / app_description.max_fps)));

            std::chrono::duration<float> elapsed_time = desired_time - actual_time;
            if (elapsed_time > std::chrono::milliseconds(0ms))
            {
                std::this_thread::sleep_for(elapsed_time);
            }
        }

        ApplicationDescription app_description;

        std::function<void()> on_initialize;
        std::function<void(const FrameInfo& info)> on_update;
        std::function<void()> on_shutdown;

        bool is_marked_for_destroy;
        bool is_running;

        static CoreApplication* s_instance;
    };

    //-------------------------------------------------------------------------
    CoreApplication* CoreApplication::Internal::s_instance = nullptr;

    //-------------------------------------------------------------------------
    CoreApplication::CoreApplication(const ApplicationDescription& description)
        :m_internal_ptr(std::make_unique<Internal>(description))
    {
        m_internal_ptr->on_initialize = [&]() { platform_initialize(); };
        m_internal_ptr->on_update = [&](const FrameInfo& info) { platform_update(info); };
        m_internal_ptr->on_shutdown = [&]() { platform_shutdown(); };

        assert(CoreApplication::Internal::s_instance == nullptr && "There can only be one application at the time");
        CoreApplication::Internal::s_instance = this;
    }
    //-------------------------------------------------------------------------
    CoreApplication::~CoreApplication()
    {
        m_internal_ptr->is_running = false;

        CoreApplication::Internal::s_instance = nullptr;
    }

    //-------------------------------------------------------------------------
    bool CoreApplication::is_running() const
    {
        return m_internal_ptr->is_running && !m_internal_ptr->is_marked_for_destroy;
    }

    //-------------------------------------------------------------------------
    int CoreApplication::run()
    {
        m_internal_ptr->initialize();
        m_internal_ptr->update();
        m_internal_ptr->shutdown();

        return EXIT_SUCCESS;
    }
    //-------------------------------------------------------------------------
    void CoreApplication::quit()
    {
        mark_for_destroy();
    }

    //-------------------------------------------------------------------------
    void CoreApplication::mark_for_destroy()
    {
        m_internal_ptr->is_marked_for_destroy = true;
    }
}