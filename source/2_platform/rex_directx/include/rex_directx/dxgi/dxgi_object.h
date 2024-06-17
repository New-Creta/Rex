#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/platform/win/win_com_ptr.h"

namespace rex
{
  namespace gfx
  {
    namespace dxgi
    {
      template <typename T>
      class DxgiObject
      {
      public:
        DxgiObject() = default;
        DxgiObject(wrl::ComPtr<T>&& object, uint32 version)
          : m_object(rsl::move(object))
          , m_version(version)
        {
        }
        DxgiObject(T* pointer, uint32 version)
          : m_object(pointer)
          , m_version(version)
        {
        }

        uint32 version() const
        {
          return m_version;
        }

        // Retrieve this com object as a different type
        template <typename To>
        wrl::ComPtr<To> as() const
        {
          wrl::ComPtr<To> pointer;
          m_object.template As<To>(&pointer);
          return pointer;
        }

        // In order to pass along " const " objects to DirectX we are able to remove the " const " with this function.
        // Since DirectX has a C API it does not know what " const " is.
        T* get() const
        {
          static_assert(rsl::is_base_of_v<IUnknown, T>, "T is not a com object!");
          return const_cast<T*>(m_object.Get()); // NOLINT(cppcoreguidelines-pro-type-const-cast)
        }

      private:
        uint32 m_version;
        wrl::ComPtr<T> m_object;
      };
    }
  } // namespace dxgi
} // namespace rex