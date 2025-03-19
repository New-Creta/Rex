#pragma once

#include "rex_engine/engine/types.h"
#include "rex_std/memory.h"
#include "rex_engine/platform/win/win_types.h"

namespace rex
{
  namespace win
  {
    class WindowClass
    {
    public:
      WindowClass();
      WindowClass(const WindowClass&) = delete;
      WindowClass(WindowClass&&)      = delete;
      ~WindowClass();

      WindowClass& operator=(const WindowClass&) = delete;
      WindowClass& operator=(WindowClass&&)      = delete;

      bool create(HInstance hInstance, WindowProcedureFunc wndProc, const char8* title);
      bool destroy();

      const char8* class_name() const;

      HInstance hinstance() const;

    private:
      struct Internal;
      rsl::unique_ptr<Internal> m_internal_ptr;
    };
  } // namespace win
} // namespace rex