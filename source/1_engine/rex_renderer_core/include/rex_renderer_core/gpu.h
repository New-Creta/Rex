#pragma once

#include "rex_engine/types.h"
#include "rex_std/ostream.h"
#include "rex_std/string.h"
#include "rex_std_extra/memory/memory_size.h"

namespace rex
{
  class Gpu
  {
  public:
    struct Description
    {
      Description();

      rsl::small_stack_string name;
      rsl::small_stack_string vendor_name;

      u32 vendor_id;
      u32 device_id;

      rsl::memory_size dedicated_video_memory;
      rsl::memory_size dedicated_system_memory;
      rsl::memory_size shared_system_memory;
    };

    explicit Gpu(const Description& desc, void* nativeHandle);
    virtual ~Gpu() = default;

    const Description& description() const;

    // this casts away the const by design
    // as the native handle is often a C object
    // that doesn't understand const.
    void* native_handle() const;

  private:
    Description m_description;
    void* m_native_handle;
  };

  rsl::ostream& operator<<(rsl::ostream& os, const Gpu& obj);
} // namespace rex