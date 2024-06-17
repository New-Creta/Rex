#pragma once

namespace rex
{
  namespace gfx
  {
    // Base class for a root signature, this will likely change in the future
    class RootSignature
    {
    public:
      virtual ~RootSignature() = default;
    };
  }
}