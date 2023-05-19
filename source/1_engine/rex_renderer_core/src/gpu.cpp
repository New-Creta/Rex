#include "rex_renderer_core/gpu.h"

#include "rex_std/string.h"

namespace rex
{
  //-------------------------------------------------------------------------
  rsl::ostream& operator<<(rsl::ostream& os, const Gpu& obj)
  {
    const Gpu::Description& desc = obj.description();

    os << "\n";
    os << "Description: " << desc.name << "\n";
    os << "Vendor Name: " << desc.vendor_name << "\n";
    os << "Vendor ID: " << desc.vendor_id << "\n";
    os << "Device ID: " << desc.device_id << "\n";
    os << "Dedicated Video Memory: " << desc.dedicated_video_memory.size_in_mb() << "MB"
       << "\n";
    os << "Dedicated System Memory: " << desc.dedicated_system_memory.size_in_mb() << "MB"
       << "\n";
    os << "Shared System Memory: " << desc.shared_system_memory.size_in_mb() << "MB"
       << "\n";

    return os;
  }

  //-------------------------------------------------------------------------
  Gpu::Gpu::Description::Description()
      : name("")
      , vendor_name("")
      , vendor_id(0)
      , device_id(0)
      , dedicated_video_memory(0)
      , dedicated_system_memory(0)
      , shared_system_memory(0)
  {
  }

  //-------------------------------------------------------------------------
  Gpu::Gpu(const Gpu::Description& desc)
      : m_description(desc)
  {
  }

  //-------------------------------------------------------------------------
  const Gpu::Description& Gpu::description() const
  {
    return m_description;
  }
} // namespace rex