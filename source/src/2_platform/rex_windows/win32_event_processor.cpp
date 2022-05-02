#include "rex_windows_pch.h"

#include "win32_event_processor.h"
#include "win32_window.h"

// window events
#include "window/window_close.h"
#include "window/window_show.h"
#include "window/window_hide.h"

namespace rex
{
    namespace win32
    {
        //-------------------------------------------------------------------------
        EventProcessor::EventProcessor(Window* window, events::EventCallbackFn callback)
            : m_window(window)
            , m_callback(callback)
            , m_enabled(false)
        {
        }
        //-------------------------------------------------------------------------
        EventProcessor::~EventProcessor() = default;

        //-------------------------------------------------------------------------
        void EventProcessor::enable_event_processing()
        {
            m_enabled = true;
        }
        //-------------------------------------------------------------------------
        void EventProcessor::disable_event_processing()
        {
            m_enabled = false;
        }

        //-------------------------------------------------------------------------
        void EventProcessor::process_window_shown(int32 windowID/*, const memory::Blob& nativeEvt*/)
        {
            if (!m_enabled)
                return;

            auto rex_evt = events::make_new<events::WindowShow>(windowID/*, nativeEvt*/);
            m_callback(rex_evt);
        }
        //-------------------------------------------------------------------------
        void EventProcessor::process_window_hide(int32 windowID/*, const memory::Blob& nativeEvt*/)
        {
            if (!m_enabled)
                return;

            auto rex_evt = events::make_new<events::WindowHide>(windowID/*, nativeEvt*/);
            m_callback(rex_evt);
        }
        //-------------------------------------------------------------------------
        void EventProcessor::process_window_close(int32 windowID/*, const memory::Blob& nativeEvt*/)
        {
            // UNUSED_PARAM(evt);

            if (!m_enabled)
                return;

            auto rex_evt = events::make_new<events::WindowClose>(windowID/*, nativeEvt*/);
            m_callback(rex_evt);
        }
    }
}