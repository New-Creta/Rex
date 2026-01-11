#pragma once

namespace rex
{
  namespace gfx
  {
    // The resource class just acts as an base class for all gpu resources
    class Resource
    {
    public:
      virtual ~Resource() = default;

      // Returns the api specific object that the resource is wrapping
      // for example, for texture on DirectX 12, this would return the ID3D12Resource*
      virtual void* api_object() const = 0;

    private:

    };
  }
}