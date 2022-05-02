#include "rex_windows_pch.h"

#include "win32_application_loop.h"

#include "world.h"

#include "frameinfo/frameinfo.h"

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        World g_world;

        //-------------------------------------------------------------------------
        ApplicationLoop::ApplicationLoop(const UpdateDelegate& updateDelegate, const RefreshRate& refreshRate)
            : m_is_running(false)
            , m_update_delegate(updateDelegate)
            , m_refresh_rate(refreshRate)
        {
        }

        //-------------------------------------------------------------------------
        ApplicationLoop::~ApplicationLoop()
        {
        }

        //-------------------------------------------------------------------------
        void ApplicationLoop::exec()
        {
            m_is_running = true;

            while (m_is_running)
            {
                tick();
            }
        }

        //-------------------------------------------------------------------------
        void ApplicationLoop::stop()
        {
            m_is_running = false;
        }

        //-------------------------------------------------------------------------
        void ApplicationLoop::tick()
        {
            FrameInfo info = {World::get_delta_time(), World::get_frames_per_second()};

            m_update_delegate(info);

            g_world.update();

            //
            // Cap framerate to "max_fps".
            // Safe resources of the machine we are running on.
            //
            std::chrono::milliseconds actual_time(static_cast<uint32>(std::lrint(1000.0f / g_world.get_frames_per_second().get())));
            std::chrono::milliseconds desired_time(static_cast<uint32>(std::lrint(1000 / m_refresh_rate.get())));
            
            std::chrono::duration<float> elapsed_time = desired_time - actual_time;
            if (elapsed_time > std::chrono::milliseconds(0ms))
            {
                std::this_thread::sleep_for(elapsed_time);
            }
        }
    }
}