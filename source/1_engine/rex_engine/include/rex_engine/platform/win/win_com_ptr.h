#pragma once

// Utility com ptr class
// We use this one for some classes instead of wrl::ComPtr<T>
// because wrl includes windows.h, and that's what we're trying to avoid

namespace rex
{
  namespace win
  {
    template <typename T>
    class ComPtr
    {
    public:
      ComPtr()
          : m_pointer(nullptr)
      {
      }

      ComPtr(T* ptr)
          : m_pointer(ptr)
      {
      }

      ~ComPtr()
      {
        m_pointer->Release();
      }

      T** get_address_of()
      {
        return &m_pointer;
      }

      T* operator->()
      {
        return m_pointer;
      }

      T* get()
      {
        return m_pointer;
      }

      const T* operator->() const
      {
        return m_pointer;
      }

      const T* get() const
      {
        return m_pointer;
      }

      explicit operator bool() const
      {
        return m_pointer != nullptr;
      }

    private:
      T* m_pointer;
    };
  } // namespace win
} // namespace rex