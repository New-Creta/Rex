#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"
#include "rex_engine/memory/alloc_unique.h"

#include "rex_std/memory.h"

namespace rex
{
	namespace internal
	{
		struct BlockHeader
		{
			BlockHeader* next;
		};
	}

	//Block allocator implementation
	template <typename BackendAllocator>
	class BlockAllocator
	{
	public:
		using size_type = s32;
		using pointer = void*;

		BlockAllocator()
			: m_buffer(nullptr)
			, m_head(nullptr)
		{
			
		}
		BlockAllocator(size_type size, size_type blockSize, BackendAllocator alloc = BackendAllocator())
		{
			m_buffer = alloc_unique<rsl::byte[]>(alloc, size);
			m_head = reinterpret_cast<internal::BlockHeader*>(m_buffer.get());
			m_head->next = nullptr;
		}

		REX_NO_DISCARD pointer allocate(size_type size, size_type alignment)
		{
			REX_UNUSED_PARAM(size); // Size cannot be used for buddy allocator
			REX_UNUSED_PARAM(alignment); // Alignment cannot be used for buddy allocator

			REX_ASSERT_X(size < m_block_size, "Trying to allocate something that's bigger than the block size of a block allocator");

			internal::BlockHeader* current = m_head;
			REX_ASSERT_X(current != nullptr, "Ran out of memory in the free list");

			remove_free_block(current);
			return to_user_pointer(current);
		}
		REX_NO_DISCARD pointer allocate(size_type size)
		{
			u64 alignment = alignof(rsl::max_align);
			return allocate(size, alignment);
		}
		template <typename T>
		REX_NO_DISCARD T* allocate()
		{
			return static_cast<T*>(allocate(sizeof(T), alignof(T)));
		}

		void deallocate(pointer ptr, size_type size)
		{
			rsl::byte* ptr_as_bytes = reinterpret_cast<rsl::byte*>(ptr);
			ptr_as_bytes -= sizeof(internal::BlockHeader);
			internal::BlockHeader* block = reinterpret_cast<internal::BlockHeader*>(ptr_as_bytes);

			block->next = m_head;
			m_head = block;
		}
		template <typename T>
		void deallocate(T* ptr)
		{
			deallocate(ptr, sizeof(T));
		}

		template <typename U, typename... Args>
		void  construct(U* p, Args&&... args)
		{
			new (p) (rsl::forward<Args>(args)...);
		}
		template <typename T>
		void destroy(T* ptr)
		{
			ptr->~T();
		}

		bool operator==(const BlockAllocator& rhs) const
		{
			return m_buffer.get() == rhs.m_buffer.get();
		}
		bool operator!=(const BlockAllocator& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		pointer to_user_pointer(internal::BlockHeader* block) const
		{
			rsl::byte* ptr = reinterpret_cast<rsl::byte*>(block);
			return ptr += sizeof(internal::BlockHeader);
		}
		void remove_free_block(internal::BlockHeader* toRemove)
		{
			m_head = toRemove->next;
		}

	private:
		rsl::unique_array<rsl::byte, DeleterWithAllocator<rsl::byte, BackendAllocator>> m_buffer;
		size_type m_block_size;
		internal::BlockHeader* m_head;
	};
}