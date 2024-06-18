#pragma once

namespace rex
{
  namespace gfx
  {
    template <card32 DirectXVersion>
    class BoundResourceReflection
    {
    public:
      BoundResourceReflection(const typename ShaderReflectionTypes<DirectXVersion>::D3D_SHADER_INPUT_BIND_DESC& resource_desc)
        : m_name(resource_desc.Name)
        , m_register(resource_desc.BindPoint)
      {}

      const ca::TinyStackString& name() const
      {
        return m_name;
      }
      card32 shader_register() const
      {
        return m_register;
      }

    private:
      ca::TinyStackString m_name;
      card32 m_register;

    };

  }
}