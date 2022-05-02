#pragma once

#include "core_application.h"

#include "event.h"

namespace rex
{
    struct FrameInfo;

    class DisplayManager;
    class LayerStack;
    class Layer;
    class CoreWindow;
    class FPS;

    namespace events
    {
        class EventQueue;

        class WindowClose;
        class WindowResize;
    }

    namespace win32
    {
        class Window;
        class ApplicationLoop;

        class Application : public CoreApplication
        {
        public:
            Application(const ApplicationDescription& description);
            ~Application() override;

            const CoreWindow* get_window() const;

            bool is_marked_for_destruction() const;

            void quit() override;

        protected:
            Layer* push_back_layer(rtl::UniquePtr<Layer> layer);

            bool platform_initialize() override;
            bool platform_shutdown() override;

            virtual void on_app_initialize();
            virtual void on_app_update(const FrameInfo& info);
            virtual void on_app_event(events::Event& event);
            virtual void on_app_shutdown();

        private:
            void platform_update(const FrameInfo& info);
            void platform_event(events::Event& event);

            void process_events();
            void process_render_queue(const FrameInfo& info);
            void process_window(const FrameInfo& info);

            void mark_for_destroy();

            void create_display_manager();
            void create_event_queue();
            void create_layer_stack();
            void create_window();
            void create_application_loop();

            bool on_window_close(const events::WindowClose& evt);

            bool m_is_marked_for_destruction;

            rtl::UniquePtr<DisplayManager> m_display_manager;
            rtl::UniquePtr<events::EventQueue> m_event_queue;
            rtl::UniquePtr<LayerStack> m_layer_stack;
            rtl::UniquePtr<Window> m_window;
            rtl::UniquePtr<ApplicationLoop> m_application_loop;
        };
    }
}