#pragma once

#include "rex_directx/wrl/comobject.h"
#include "rex_engine/platform/win/win_com_ptr.h"

namespace rex
{
  namespace gfx
  {
    namespace dxgi
    {
      template <typename T>
      class DxgiObject : public wrl::ComObject<T>
      {
      public:
        DxgiObject(wrl::ComPtr<T>&& object, uint32 version);
        DxgiObject(T* pointer, uint32 version);

        uint32 version() const;

      private:
        uint32 m_version;
      };

      //-------------------------------------------------------------------------
      template <typename T>
      DxgiObject<T>::DxgiObject(wrl::ComPtr<T>&& object, uint32 version)
        : wrl::ComObject<T>(rsl::move(object))
        , m_version(version)
      {
      }
      //-------------------------------------------------------------------------
      template <typename T>
      DxgiObject<T>::DxgiObject(T* pointer, uint32 version)
        : wrl::ComObject<T>(pointer)
        , m_version(version)
      {
      }

      //-------------------------------------------------------------------------
      template <typename T>
      uint32 DxgiObject<T>::version() const
      {
        return m_version;
      }
    }
  } // namespace dxgi
} // namespace rex