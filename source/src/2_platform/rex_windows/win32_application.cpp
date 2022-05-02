#include "rex_windows_pch.h"

#include "win32_application.h"
#include "win32_application_loop.h"
#include "win32_window.h"

#include "layer.h"
#include "layer_stack.h"

#include "event_system.h"

#include "world.h"
#include "frameinfo/frameinfo.h"
#include "frameinfo/fps.h"

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        Application::Application(const ApplicationDescription& description)
            : CoreApplication(description)
            , m_is_marked_for_destruction(false)
        {
        }

        //-------------------------------------------------------------------------
        Application::~Application()
        {
        }

        //-------------------------------------------------------------------------
        const CoreWindow* Application::get_window() const
        {
            return m_window.get();
        }

        //-------------------------------------------------------------------------
        bool Application::is_marked_for_destruction() const
        {
            return m_is_marked_for_destruction;
        }

        //-------------------------------------------------------------------------
        void Application::quit()
        {
            if (is_marked_for_destruction())
            {
                // R_ERROR("Application already prepared for destruction.");

                return;
            }

            mark_for_destroy();
        }

        //-------------------------------------------------------------------------
        Layer* Application::push_back_layer(rtl::UniquePtr<Layer> layer)
        {
            Layer* raw_ptr = layer.get();

            m_layer_stack->push(std::move(layer));

            return raw_ptr;
        }

        //-------------------------------------------------------------------------
        bool Application::platform_initialize()
        {
            // R_INFO("[APPLICATION] Successfully initialized SDL!");

            create_layer_stack();
            create_event_queue();
            create_window();
            create_application_loop();

            int32 window_width = m_window->width();
            int32 window_height = m_window->height();

            on_app_initialize();

            m_window->show();

            // Run the main application loop!
            m_application_loop->exec();

            return true;
        }

        //-------------------------------------------------------------------------
        bool Application::platform_shutdown()
        {
            on_app_shutdown();

            m_layer_stack->clear();
            m_layer_stack.reset();

            m_application_loop.reset();

            m_window.reset();

            return true;
        }

        //-------------------------------------------------------------------------
        void Application::on_app_initialize()
        {
            // Implement in derived class
        }

        //-------------------------------------------------------------------------
        void Application::on_app_update(const FrameInfo& /* info */)
        {
            // Implement in derived class
        }

        //-------------------------------------------------------------------------
        void Application::on_app_event()
        {
            // Implement in derived class
        }

        //-------------------------------------------------------------------------
        void Application::on_app_shutdown()
        {
            // Implement in derived class
        }

        //-------------------------------------------------------------------------
        void Application::platform_update(const FrameInfo& info)
        {
            process_events();
            process_render_queue(info);
            process_window(info);
        }
        //-------------------------------------------------------------------------
        void Application::platform_event()
        {
            std::for_each(m_layer_stack->rbegin(), m_layer_stack->rend(), [](rtl::UniquePtr<Layer>& layer) mutable
                          {
                              layer->handle_event();
                          });

            on_app_event();
        }

        //-------------------------------------------------------------------------
        void Application::process_events()
        {

        }
        //-------------------------------------------------------------------------
        void Application::process_render_queue(const FrameInfo& info)
        {
            // bool is_visible = m_window->is_visible();
            constexpr bool is_visible = true;

            if (is_visible)
            {
                LayerStack& layer_stack = *m_layer_stack;
                for (rtl::UniquePtr<Layer>& layer : layer_stack)
                {
                    if (layer->is_enabled())
                    {
                        layer->update(info);
                    }
                }

                on_app_update(info);
            }
        }
        //-------------------------------------------------------------------------
        void Application::process_window(const FrameInfo& info)
        {
            m_window->update();
        }

        //-------------------------------------------------------------------------
        void Application::mark_for_destroy()
        {
            m_is_marked_for_destruction = true;

            m_application_loop->stop();

            // R_INFO("[APPLICATION] Mark application for destruction.");
        }

        //-------------------------------------------------------------------------
        void Application::create_layer_stack()
        {
            m_layer_stack = rtl::make_unique<LayerStack>();
        }

        //-------------------------------------------------------------------------
        void Application::create_event_queue()
        {
            event_system::subscribe(event_system::EventType::WindowClose, [this]() { mark_for_destroy(); });
        }

        //-------------------------------------------------------------------------
        void Application::create_window()
        {
            WindowDescription window_description;

            window_description.title = "rex";
            window_description.width = get_application_description().window_width;
            window_description.height = get_application_description().window_height;

            m_window = rtl::make_unique<Window>(window_description);

            // R_INFO("[APPLICATION] Window ({0}, {1}) intialized.", m_window->get_width(), m_window->get_height());
        }

        //-------------------------------------------------------------------------
        void Application::create_application_loop()
        {
            constexpr RefreshRate refresh_rate = 60_hz;

            auto fn = [&](const FrameInfo& info)
            {
                platform_update(info);
            };

            m_application_loop = rtl::make_unique<ApplicationLoop>(fn, refresh_rate);
        }
    } // namespace win32
}