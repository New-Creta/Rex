#pragma once

#include "rex_engine/platform/win/diagnostics/hr_call.h"
#include "rex_engine/platform/win/win_com_ptr.h"
#include "rex_std/bonus/string.h"
#include "rex_std/bonus/types.h"

#include <Windows.h>
#include <d3d12.h>
#include <stddef.h>

namespace rex::win
{
  class ComLibrary
  {
  public:
    ComLibrary();
    ~ComLibrary();

    ComLibrary(const ComLibrary&) = delete;
    ComLibrary(ComLibrary&&)      = delete;

    ComLibrary& operator=(const ComLibrary&) = delete;
    ComLibrary& operator=(ComLibrary&&)      = delete;

    bool is_initialized() const;

    template <typename ComObject>
    ComPtr<ComObject> create_com_object()
    {
      ComPtr<ComObject> com_object;
      HR_CALL(CoCreateInstance(__uuidof(ComObject), NULL, CLSCTX_ALL, IID_PPV_ARGS(com_object.get_address_of())));
      return com_object;
    }

    template <typename ComObject>
    ComPtr<ComObject> create_com_object(IID id)
    {
      ComPtr<ComObject> com_object;
      HR_CALL(CoCreateInstance(id, NULL, CLSCTX_ALL, IID_PPV_ARGS(com_object.get_address_of())));
      return com_object;
    }

    rsl::string read_link(rsl::string_view filepath);

  private:
    bool init_lib();
    void uninit_lib();

    void inc_ref();
    void dec_ref();

  private:
    static thread_local card32 s_init_succeeded_count;
  };

  bool init_com_library();
  ComLibrary& com_library();
} // namespace rex::win
