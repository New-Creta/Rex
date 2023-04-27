#include "rex_directx/dxgi/comobject.h"
#include "rex_directx/wrl/wrl_types.h"
#include "rex_engine/types.h"
#include "rex_std/memory.h"

struct IDXGIFactory;

namespace rex
{
  namespace dxgi
  {
    class Factory : public ComObject<IDXGIFactory>
    {
    public:
      static rsl::unique_ptr<Factory> create();

      Factory(wrl::com_ptr<IDXGIFactory>&& object, uint32 version);
    };
  } // namespace dxgi
} // namespace rex