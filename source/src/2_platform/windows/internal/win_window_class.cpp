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
            : window_class()
            , name(name)
            , hinstance((HINSTANCE)GetModuleHandleA(NULL))
        {
            ZeroMemory(&window_class, sizeof(window_class));

            REX_TODO("Make window style data driven");
            REX_TODO("Make window icon data driven");
            REX_TODO("Make window cursor data driven");

            window_class.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
            window_class.lpfnWndProc = (WNDPROC)wnd_proc;
            window_class.hInstance = (HINSTANCE)GetModuleHandleA(NULL);
            window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
            window_class.hCursor = LoadCursor(NULL, IDC_ICON);
            window_class.lpszClassName = name.data();

            if (WIN_FAILED(RegisterClass(&window_class)))
            {
                REX_ERROR("Failed to create window class!");
            }
        }
        ~Internal()
        {
            if (WIN_FAILED(UnregisterClass(name.data(), (HINSTANCE)hinstance)))
            {
                REX_ERROR("Failed to destroy window class!");
            }
        }

        WNDCLASS window_class;
        rtl::StringView name;
        HInstance hinstance;
    };
}

//-----------------------------------------------------------------
rex::win32::WindowClass::WindowClass(const rtl::StringView name, WindowProcedureFunc wnd_proc)
    : m_internal_ptr(rtl::make_unique<Internal>(name, wnd_proc))
{}

const rtl::StringView rex::win32::WindowClass::class_name() const
{
    return m_internal_ptr->name;
}

rex::win32::HInstance rex::win32::WindowClass::hinstance() const
{
    return m_internal_ptr->hinstance;
}