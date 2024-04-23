#include "rex_renderer_core/clear_bits.h"

namespace rex
{
    namespace renderer
    {
      //-------------------------------------------------------------------------
      bool operator&(ClearBits bits1, ClearBits bits2)
      {
        return (static_cast<u32>(bits1) & static_cast<u32>(bits2)) != 0;
      }

      //-------------------------------------------------------------------------
      bool operator|(ClearBits bits1, ClearBits bits2)
      {
        return (static_cast<u32>(bits1) | static_cast<u32>(bits2)) != 0;
      }
    }
}