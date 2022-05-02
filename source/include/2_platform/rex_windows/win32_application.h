#pragma once

#include "core_application.h"

namespace rex
{
    struct FrameInfo;

    class LayerStack;
    class Layer;
    class CoreWindow;
    class FPS;

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
            virtual void on_app_event();
            virtual void on_app_shutdown();

        private:
            void platform_update(const FrameInfo& info);
            void platform_event();

            void process_events();
            void process_render_queue(const FrameInfo& info);
            void process_window(const FrameInfo& info);

            void mark_for_destroy();

            void create_layer_stack();
            void create_event_queue();
            void create_window();
            void create_application_loop();

            bool m_is_marked_for_destruction;

            rtl::UniquePtr<LayerStack> m_layer_stack;
            rtl::UniquePtr<Window> m_window;
            rtl::UniquePtr<ApplicationLoop> m_application_loop;
        };
    }
}