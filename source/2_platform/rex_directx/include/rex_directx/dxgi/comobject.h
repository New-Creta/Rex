#pragma once

#include "rex_directx/wrl/comobject.h"
#include "rex_engine/win/win_com_ptr.h"

namespace rex
{
  namespace dxgi
  {
    template <typename T>
    class ComObject : public wrl::ComObject<T>
    {
    public:
      ComObject(wrl::ComPtr<T>&& object, uint32 version);
      ComObject(T* pointer, uint32 version);

      uint32 version() const;

    private:
      uint32 m_version;
    };

    //-------------------------------------------------------------------------
    template <typename T>
    ComObject<T>::ComObject(wrl::ComPtr<T>&& object, uint32 version)
        : wrl::ComObject<T>(rsl::move(object))
        , m_version(version)
    {
    }
    //-------------------------------------------------------------------------
    template <typename T>
    ComObject<T>::ComObject(T* pointer, uint32 version)
        : wrl::ComObject<T>(pointer)
        , m_version(version)
    {
    }

    //-------------------------------------------------------------------------
    template <typename T>
    uint32 ComObject<T>::version() const
    {
      return m_version;
    }
  } // namespace dxgi
} // namespace rex