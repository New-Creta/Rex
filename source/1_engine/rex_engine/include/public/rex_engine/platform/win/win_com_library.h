#pragma once

#include "rex_engine/memory/memory_types.h"
#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

#include <Windows.h>
#include <d3d12.h>
#include <stddef.h>

namespace rex::win
{
  namespace com_lib
  {
    // Creates a WinComHandle.
    // Makes sure the win com lib is initialized before creating the handle
    class WinComLibHandle create_lib_handle();

    // A WinComLibHandle makes sure the win com lib gets destroyed
    // when the handle goes out of scope
    class WinComLibHandle
    {
    public:
      WinComLibHandle() = default;
      WinComLibHandle(const WinComLibHandle& /*other*/);
      WinComLibHandle(WinComLibHandle&&) = default;
      ~WinComLibHandle();

      WinComLibHandle& operator=(const WinComLibHandle&) = default;
      WinComLibHandle& operator=(WinComLibHandle&&) = default;
    };

    // Return if the win com lib is initialized already or not
    bool is_initialized();

    // Read a symbolic link's path and return the path it actually points to
    scratch_string read_link(rsl::string_view filepath);

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
      HR_CALL(CoCreateInstance(id, nullptr, CLSCTX_ALL, IID_PPV_ARGS(com_object.GetAddressOf())));
      return com_object;
    }
  } // namespace com_lib
} // namespace rex::win
