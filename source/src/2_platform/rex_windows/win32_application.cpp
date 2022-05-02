#include "rex_windows_pch.h"

#include "win32_application.h"
#include "win32_application_loop.h"
#include "win32_window.h"

#include "layer.h"
#include "layer_stack.h"

#include "event_dispatcher.h"
#include "event_queue.h"
#include "event_bus.h"

#include "window/window_close.h"

#include "display.h"
#include "display_manager.h"

#include "world.h"
#include "frameinfo/frameinfo.h"
#include "frameinfo/fps.h"

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        const int32 g_main_display = 0;
        const int32 g_main_display_mode = 0;

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

            create_display_manager();
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

            events::EventBus::destroy_instance();

            m_application_loop.reset();

            m_window.reset();
            m_display_manager.reset();

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
        void Application::on_app_event(events::Event& event)
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
        void Application::platform_event(events::Event& event)
        {
            std::for_each(m_layer_stack->rbegin(), m_layer_stack->rend(), [&event](rtl::UniquePtr<Layer>& layer) mutable
                          {
                              layer->handle_event(event);
                          });

            on_app_event(event);

            events::EventDispatcher dispatcher(event);
            dispatcher.dispatch<events::WindowClose>([&](const events::WindowClose& closeEvent)
                                                     {
                                                         return on_window_close(closeEvent);
                                                     });
        }

        //-------------------------------------------------------------------------
        void Application::process_events()
        {
            if (!m_event_queue->empty())
            {
                int32 pump_count = 0;
                events::Event evt = m_event_queue->next();
                while (evt != nullptr && pump_count < events::EventQueue::EVENT_QUEUE_PUMP_COUNT)
                {
                    platform_event(evt);

                    evt = m_event_queue->next();
                    ++pump_count;
                }
            }
        }
        //-------------------------------------------------------------------------
        void Application::process_render_queue(const FrameInfo& info)
        {
            bool is_visible = m_window->is_visible();

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
        void Application::create_display_manager()
        {
            m_display_manager = rtl::make_unique<DisplayManager>();
            m_display_manager->set_active(g_main_display, g_main_display_mode);

            //R_INFO("[APPLICATION] Display manager initialized with {0} display(s).", m_display_manager->display_count());
        }

        //-------------------------------------------------------------------------
        void Application::create_layer_stack()
        {
            m_layer_stack = rtl::make_unique<LayerStack>();
        }

        //-------------------------------------------------------------------------
        void Application::create_event_queue()
        {
            // event_system::subscribe(event_system::EventType::WindowClose, [this]() { mark_for_destroy(); });

            m_event_queue = rtl::make_unique<events::EventQueue>();

            events::EventBus::create_instance();
            events::EventBus::instance()->push_event_queue(m_event_queue.get(), events::EventBusImpl::Activate::Yes);
        }

        //-------------------------------------------------------------------------
        void Application::create_window()
        {
            WindowDescription window_description;

            window_description.title = "rex";
            window_description.width = get_application_description().window_width;
            window_description.height = get_application_description().window_height;
            window_description.display = m_display_manager->get_active();
            window_description.event_callback = [&event_queue = *m_event_queue](events::Event& event)
            {
                event_queue.enqueue(event);
            };

            m_window = rtl::make_unique<Window>(window_description);

            // R_INFO("[APPLICATION] Window ({0}, {1}) intialized.", m_window->get_width(), m_window->get_height());
        }

        //-------------------------------------------------------------------------
        void Application::create_application_loop()
        {
            auto display = m_display_manager->get_active();
            auto display_mode = display->get_active_mode();

            auto fn = [&](const FrameInfo& info)
            {
                platform_update(info);
            };

            m_application_loop = rtl::make_unique<ApplicationLoop>(fn, RefreshRate(display_mode->get_refresh_rate()));

            // R_INFO("[APPLICATION] Application loop ({0} hz) initialized.", display_mode->get_refresh_rate());
        }

        //-------------------------------------------------------------------------
        bool Application::on_window_close(const events::WindowClose& evt)
        {
            // uint32 main_window_id = SDL_GetWindowID(m_window->get_sdl_window());
            // if (main_window_id != evt.get_window_id())
            // {
            //     return false;
            // }

            mark_for_destroy();

            return true;
        }
    } // namespace win32
}