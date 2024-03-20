#include "rex_directx/system/directx_command_queue.h"

namespace rex
{
  namespace rhi
  {
    CommandQueue::CommandQueue(const wrl::ComPtr<ID3D12CommandQueue>& commandQueue)
      : m_command_queue(commandQueue)
    {}
  }
}