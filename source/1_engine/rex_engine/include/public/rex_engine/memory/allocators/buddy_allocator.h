#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

#include "rex_std/memory.h"

namespace rex
{
	namespace internal
	{
		struct BuddyBlockHeader
		{
			s32 size;
			BuddyBlockHeader* next;
		};
	}

	//Buddy allocator implementation
	template <typename BackendAllocator>
	class BuddyAllocator
	{
	public:
		using size_type = s32;
		using pointer = void*;

		BuddyAllocator()
			: m_buffer(nullptr)
			, m_head()
		{}
		explicit BuddyAllocator(size_type size, BackendAllocator alloc = BackendAllocator())
		{
			m_buffer = alloc_unique<rsl::byte[]>(alloc, size);
			m_head = reinterpret_cast<internal::BuddyBlockHeader*>(m_buffer.get());
			m_head->size = m_buffer.count() - sizeof(internal::BuddyBlockHeader);
			m_head->next = nullptr;
		}

		REX_NO_DISCARD pointer allocate(size_type size, size_type alignment)
		{
			REX_UNUSED_PARAM(alignment); // Alignment cannot be used for buddy allocator

			internal::BuddyBlockHeader* prev = nullptr;
			internal::BuddyBlockHeader* current = m_head;

			while (current)
			{
				if (current->size >= size)
				{
					if (can_split_block(current, size))
					{
						split_block(current, size);
					}
					else
					{
						remove_free_block(prev, current);
					}

					return to_user_pointer(current);
				}

				prev = current;
				current = current->next;
			}

			REX_ASSERT("Ran out of memory in the free list");
			return nullptr;
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
			ptr_as_bytes -= sizeof(internal::BuddyBlockHeader);
			internal::BuddyBlockHeader* block = reinterpret_cast<internal::BuddyBlockHeader*>(ptr_as_bytes);

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

		bool operator==(const BuddyAllocator& rhs) const
		{
			return m_buffer.get() == rhs.m_buffer.get();
		}
		bool operator!=(const BuddyAllocator& rhs) const
		{
			return !(*this == rhs);
		}

	private:
		pointer to_user_pointer(internal::BuddyBlockHeader* block) const
		{
			rsl::byte* ptr = reinterpret_cast<rsl::byte*>(block);
			return ptr += sizeof(internal::BuddyBlockHeader);
		}
		bool can_split_block(const internal::BuddyBlockHeader* block, size_type size) const
		{
			return block->size - size > sizeof(internal::BuddyBlockHeader);
		}
		void split_block(internal::BuddyBlockHeader* block, size_type size) const
		{
			rsl::byte* block_as_bytes = reinterpret_cast<rsl::byte*>(block);
			block_as_bytes += sizeof(internal::BuddyBlockHeader) + size;
			internal::BuddyBlockHeader* new_block = reinterpret_cast<internal::BuddyBlockHeader*>(block_as_bytes);
			new_block->size = block->size - sizeof(internal::BuddyBlockHeader) - size;
			new_block->next = block->next;

			block->size = size;
			block->next = new_block;
		}
		void remove_free_block(internal::BuddyBlockHeader* prev, internal::BuddyBlockHeader* toRemove)
		{
			if (prev)
			{
				prev->next = toRemove->next;
			}
			else
			{
				m_head = toRemove->next;
			}
		}

	private:
		rsl::unique_array<rsl::byte> m_buffer;
		internal::BuddyBlockHeader* m_head;
	};
}