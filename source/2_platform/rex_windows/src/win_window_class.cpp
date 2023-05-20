#include "rex_windows/win_window_class.h"

#include "rex_engine/diagnostics/win/win_call.h"
#include "rex_windows/log.h"

#include "rex_engine/diagnostics/logging/log_macros.h"
#include "rex_std/bonus/memory.h"

#define NOMINMAX
#include <Windows.h>

namespace rex
{
  namespace win32
  {
    struct WindowClass::Internal
    {
      Internal()
          : window_class()
          , name()
          , hinstance(nullptr)
          , registered(false)
      {
      }

      bool create(HInstance hInstance, WindowProcedureFunc wndProc, const char8* title)
      {
        name      = title;
        hinstance = (hInstance == nullptr) ? static_cast<HInstance>(GetModuleHandle(nullptr)) : hInstance;

        rsl::zero_memory(&window_class, sizeof(window_class));

        REX_WARN(LogWindows, "Make window style data driven");
        REX_WARN(LogWindows, "Make window icon data driven");
        REX_WARN(LogWindows, "Make window cursor data driven");

        window_class.style         = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc   = reinterpret_cast<WNDPROC>(wndProc); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        window_class.cbClsExtra    = 0;
        window_class.cbWndExtra    = 0;
        window_class.hInstance     = static_cast<HINSTANCE>(hinstance);
        window_class.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
        window_class.hCursor       = LoadCursor(nullptr, IDC_ICON);
        window_class.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOWFRAME); // NOLINT(cppcoreguidelines-pro-type-reinterpret-cast)
        window_class.lpszClassName = name;
        window_class.lpszMenuName  = nullptr;

        // RegisterClass doesn't call SetLastError on success.
        // so if anything fails before this is called, then the following call will also fail
        // please check if the issue is not coming from somewhere else.
        if(WIN_FAILED(RegisterClass(&window_class)))
        {
          REX_ERROR(LogWindows, "Failed to create window class!");
          return false;
        }
        else
        {
          registered = true;
        }

        return true;
      }

      bool destroy()
      {
        if(WIN_FAILED(UnregisterClass(name, static_cast<HINSTANCE>(hinstance))))
        {
          REX_ERROR(LogWindows, "Failed to destroy window class!");
          return false;
        }
        else
        {
          registered = false;
        }

        return true;
      }

      WNDCLASS window_class;
      const char8* name;
      HInstance hinstance;
      bool registered;
    };

    //-----------------------------------------------------------------
    WindowClass::WindowClass()
        : m_internal_ptr(rsl::make_unique<Internal>())
    {
    }

    //-------------------------------------------------------------------------
    WindowClass::~WindowClass() = default;

    //-------------------------------------------------------------------------
    bool WindowClass::create(HInstance hInstance, WindowProcedureFunc wndProc, const char8* title)
    {
      return m_internal_ptr->create(hInstance, wndProc, title);
    }

    //-------------------------------------------------------------------------
    bool WindowClass::destroy()
    {
      return m_internal_ptr->destroy();
    }

    //-------------------------------------------------------------------------
    const char8* WindowClass::class_name() const
    {
      return m_internal_ptr->name;
    }

    //-------------------------------------------------------------------------
    HInstance WindowClass::hinstance() const
    {
      return m_internal_ptr->hinstance;
    }
  } // namespace win32
} // namespace rex