#include "rex_windows/win_window_class.h"

#include "rex_engine/diagnostics/logging.h"

#include "rex_engine/diagnostics/win/win_call.h"

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
        , hinstance(NULL)
        , registered(false)
      {

      }

      bool create(HInstance hInstance, WindowProcedureFunc wnd_proc, const char8* title)
      {
        name = title;
        hinstance = hInstance == NULL ? (HInstance)GetModuleHandleA(NULL) : hInstance;

        ZeroMemory(&window_class, sizeof(window_class));

        REX_TODO("Make window style data driven");
        REX_TODO("Make window icon data driven");
        REX_TODO("Make window cursor data driven");

        window_class.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        window_class.lpfnWndProc = (WNDPROC)wnd_proc;
        //window_class.cbClsExtra = 0;
        //window_class.cbWndExtra = 0;
        //window_class.hInstance = (HINSTANCE)hinstance;
        window_class.hInstance = (HINSTANCE)GetModuleHandleA(NULL);
        window_class.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        window_class.hCursor = LoadCursor(NULL, IDC_ICON);
        //window_class.hbrBackground = (HBRUSH)(COLOR_WINDOWFRAME);
        window_class.lpszClassName = name;
        //window_class.lpszMenuName = nullptr;

        if (WIN_FAILED(RegisterClass(&window_class)))
        {
          REX_ERROR("Failed to create window class!");
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
        if (WIN_FAILED(UnregisterClass(name, (HINSTANCE)hinstance)))
        {
          REX_ERROR("Failed to destroy window class!");
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
    {}

    //-------------------------------------------------------------------------
    WindowClass::~WindowClass()
    {
      REX_ASSERT_X(m_internal_ptr->registered == false, "Explicitly destroy the window class by calling WindowClass::destroy");
    }

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
  }
}