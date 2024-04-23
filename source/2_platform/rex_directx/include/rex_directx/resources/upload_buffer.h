#pragma once

#include "rex_engine/engine/types.h"

#include "rex_directx/directx_util.h"

#include "rex_renderer_core/iresource.h"

#include "rex_std/bonus/memory/memory_size.h"
#include "rex_std/memory.h"
#include "rex_std/queue.h"
#include "rex_std/deque.h"

namespace rex
{
    namespace renderer
    {
        class Device;

        class UploadBuffer : public IResource
        {
        public:
            RESOURCE_CLASS_TYPE(UploadBuffer);

            // Used to upload data to the GPU
            struct Allocation
            {
                void* CPU;
                D3D12_GPU_VIRTUAL_ADDRESS GPU;
            };

            /**
            * The maximum size of an allocation is the size of a single page.
            */
            s64 page_size() const;

            /**
             * Allocate memory in an Upload heap.
             * An allocation must not exceed the size of a page.
             * Use a memcpy or similar method to copy the
             * buffer data to CPU pointer in the Allocation structure returned from
             * this function.
             */
            Allocation allocate(rsl::memory_size sizeInBytes, s64 alignment);

            /**
             * Release all allocated pages. This should only be done when the command list
             * is finished executing on the CommandQueue.
             */
            void reset();

        protected:
            friend struct rsl::default_delete<UploadBuffer>;

            /**
            * @param pageSize The size to use to allocate new pages in GPU memory.
            */
            explicit UploadBuffer(Device& device, rsl::memory_size pageSize = 2_mb);
            virtual ~UploadBuffer();

        private:
            // A single page for the allocator.
            class Page
            {
            public:
                Page(Device& device, rsl::memory_size sizeInBytes);
                ~Page();

                // Check to see if the page has room to satisfy the requested
                // allocation.
                bool has_space(rsl::memory_size sizeInBytes, s64 alignment) const;

                // Allocate memory from the page.
                // Throws rsl::bad_alloc if the the allocation size is larger
                // that the page size or the size of the allocation exceeds the 
                // remaining space in the page.
                Allocation allocate(rsl::memory_size sizeInBytes, s64 alignment);

                // reset the page for reuse.
                void reset();
            private:
                Device& m_device;
                wrl::ComPtr<ID3D12Resource> m_d3d12_resource;

                // Base pointer.
                void* m_CPU_ptr;
                D3D12_GPU_VIRTUAL_ADDRESS m_GPU_ptr;

                // Allocated page size.
                rsl::memory_size m_page_size;
                // Current allocation offset in bytes.
                s64 m_offset;
            };

        private:
            // Request a page from the pool of available pages
            // or create a new page if there are no available pages.
            rsl::shared_ptr<Page> request_page();

        private:
            // A pool of memory pages.
            using PagePool = rsl::deque< rsl::shared_ptr<Page>>;

            // The device that was used to create this upload buffer.
            Device& m_device;

            PagePool m_page_pool;
            PagePool m_available_pages;

            rsl::shared_ptr<Page> m_current_page;

            rsl::memory_size m_page_size;
        };
    }
}