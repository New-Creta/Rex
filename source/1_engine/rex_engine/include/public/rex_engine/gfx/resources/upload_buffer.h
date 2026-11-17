#pragma once

#include "rex_engine/gfx/core/texture_format.h"
#include "rex_engine/gfx/resources/buffer.h"
#include "rex_engine/engine/types.h"
#include "rex_std/bonus/memory.h"

#include "rex_std/functional.h"
#include "rex_std/span.h"

// DATA UPLOAD STRATEGY
// Textures are added to the front of the buffer
// Buffers are added at the back
// This is to reduce losing bytes when buffers and textures are added in a random order
// Consider the following scenario
// Note: DirectX requires 512 byte alignment for texture data
//
//  +------------------------------------------------------------------------------------------------------------+
//  |              |             |                |              |             |                |                |
//  |   256 byte   | 256 unused  |  1KiB texture  |   256 byte   | 256 unused  |  1KiB texture  |     Unused     |
//  |  buffer data |    data     |       data     |  buffer data |    data     |       data     |      data      |
//  |              |             |                |              |             |                |                |
//  +------------------------------------------------------------------------------------------------------------+
//                        ^                                             ^
//                        |                                             |
//                        +--------------------+------------------------+
//                                             |
//                                             |
//                                512 bytes that are unused
//
// As textures width and height are always powers of 2 (but not necessarily equal to each other), 
// they'll likely be 512 byte aligned, even when compressed using BC7 (please correct this comment if this isn't the case)
// As such, we can remove the wasted padding space by allocating textures to 1 end and buffers to the other.
// We chose to allocate buffers at the end and textures at the front, if this causes performance issues due to caching, this can be re-evaluated
// With buffers at the back the above example looks like this
//
//
//  +----------------------------------------------------------------------------------------------+
//  |                |                |                              |              |              |
//  |  1KiB texture  |  1KiB texture  |           Unused             |   256 byte   |   256 byte   |
//  |       data     |       data     |            data              |  buffer data |  buffer data |
//  |                |                |                              |              |              |
//  +----------------------------------------------------------------------------------------------+
//      Texture storage grows this way                            Buffer storage grows this way
//    ==================================>                       <================================
//




namespace rex
{
  namespace gfx
  {
    class UploadBuffer;
    class UploadDataAllocation
    {
    public:
      UploadDataAllocation(UploadBuffer* uploadBuffer, rsl::span<rsl::byte> allocation, s64 writeOffset);

      rsl::byte& UploadDataAllocation::operator[](s32 idx);
			const rsl::byte& UploadDataAllocation::operator[](s32 idx) const;

      rsl::byte* data();
      s64 size() const;
      s64 write_offset() const;
      bool is_from_buffer(UploadBuffer* uploadBuffer) const;

    private:
      // The upload buffer the allocation was made from
      UploadBuffer* m_upload_buffer;

      // The allocation block itself
      rsl::span<rsl::byte> m_allocation;

      // The write offset into the buffer where the allocation starts
      s64 m_write_offset;
    };

    // Interface for an api agnostic upload buffer
    class UploadBuffer : public Buffer
    {
    public:
      explicit UploadBuffer(rsl::memory_size size);
      virtual ~UploadBuffer() = default;

      // Reset the offset back to 0 so new allocations would happen at the start
      void reset();

      // Write data on cpu side, it returns the offset into the upload buffer where data was written to
      virtual s64 stage_buffer_data(const void* data, s64 size, s32 alignment = 1) = 0;
      // Write data on cpu side, it returns the offset into the upload buffer where data was written to
      virtual s64 stage_texture_data(const void* data, s32 width, s32 height, TextureFormat format) = 0;

      // Allocate an area within the upload buffer so the user can manually copy into this data
      virtual UploadDataAllocation allocate_data(s64 size, s32 alignment = 1) = 0;

    protected:
      // Return if the buffer can support an allocation of a given size and allignment
      bool can_fit_alloc(rsl::memory_size allocSize, s32 alignment);

      // Return the offset buffer the last buffer data was written to
      s64 current_buffer_offset() const;
      // Return the offset the next texture data can go to
      s64 current_texture_offset() const;

      // Decrease the offset of where buffer data goes
      s64 dec_buffer_offset(s64 size);
      // Increase the offset of where texture data goes
      s64 inc_texture_offset(s64 size, s32 alignment);

    private:
      // Textures are written at the front of the buffer
      // other buffers that don't need alignment are written at the back
      // This is to avoid unused padding space within an upload buffer
      // if buffers and textures are added in random order
      s64 m_texture_write_offset;
      s64 m_buffer_write_offset;
    };
  }
}