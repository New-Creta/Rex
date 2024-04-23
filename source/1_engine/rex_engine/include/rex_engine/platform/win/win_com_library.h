#pragma once

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <d3d12.h>
#include <stddef.h>

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_engine/platform/win/win_com_ptr.h"

namespace rex::win
{
  namespace com_lib
  {
    // A WinComLibHandle makes sure the win com lib gets destroyed
    // when the handle goes out of scope
    class WinComLibHandle
    {
    public:
      ~WinComLibHandle();
    };

    // Creates a WinComHandle.
    // Makes sure the win com lib is initialized before creating the handle
    WinComLibHandle create_lib_handle();

    // Return if the win com lib is initialized already or not
    bool is_initialized();

    // Read a symbolic link's path and return the path it actually points to
    rsl::string read_link(rsl::string_view filepath);

    // Create a com object using the com lib
    template <typename ComObject>
    wrl::ComPtr<ComObject> create_com_object()
    {
      wrl::ComPtr<ComObject> com_object;
      HR_CALL(CoCreateInstance(__uuidof(ComObject), NULL, CLSCTX_ALL, IID_PPV_ARGS(com_object.GetAddressOf())));
      return com_object;
    }

    // Create a com object using the com lib
    template <typename ComObject>
    wrl::ComPtr<ComObject> create_com_object(IID id)
    {
      wrl::ComPtr<ComObject> com_object;
      HR_CALL(CoCreateInstance(id, NULL, CLSCTX_ALL, IID_PPV_ARGS(com_object.GetAddressOf())));
      return com_object;
    }
  }
} // namespace rex::win
