#include "internal/win_window_class.h"

#include <rex_stl/diagnostics/win/win_call.h>
#include <rex_stl/diagnostics/logging.h>

#define NOMINMAX
#include <Windows.h>

namespace rex::win32
{
    struct WindowClass::Internal
    {
        Internal(const rtl::StringView name, WindowProcedureFunc wnd_proc)
            : m_window_class()
            , m_name(name)
            , m_hinstance((HINSTANCE)GetModuleHandleA(NULL))
        {
            ZeroMemory(&m_window_class, sizeof(m_window_class));

            REX_TODO("Make window style data driven");
            REX_TODO("Make window icon data driven");
            REX_TODO("Make window cursor data driven");

            m_window_class.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            m_window_class.lpfnWndProc = (WNDPROC)wnd_proc;
            m_window_class.hInstance = m_hinstance;
            m_window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            m_window_class.hCursor = LoadCursor(NULL, IDC_ICON);
            m_window_class.lpszClassName = name.data();

            if (WIN_FAILED(RegisterClass(&m_window_class)))
            {
                REX_ERROR("Failed to create window class!");
            }
        }
        ~Internal()
        {
            if (WIN_FAILED(UnregisterClass(m_name.data(), (HINSTANCE)m_hinstance)))
            {
                REX_ERROR("Failed to destroy window class!");
            }
        }

        WNDCLASS m_window_class;
        rtl::StringView m_name;
        HInstance m_hinstance;
    };
}

//-----------------------------------------------------------------
rex::win32::WindowClass::WindowClass(const rtl::StringView name, WindowProcedureFunc wnd_proc)
    : m_internal_ptr(std::make_unique<Internal>(wnd_proc))
{}