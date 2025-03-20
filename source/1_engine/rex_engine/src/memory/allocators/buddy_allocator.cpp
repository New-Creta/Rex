#include "rex_engine/memory/allocators/buddy_allocator.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	BuddyAllocator::BuddyAllocator(rsl::unique_array<rsl::byte> buffer)
		: m_buffer(rsl::move(buffer))
		, m_head()
	{
		m_head = reinterpret_cast<internal::BuddyBlockHeader*>(m_buffer.get());
		m_head->size = m_buffer.count() - sizeof(internal::BuddyBlockHeader);
		m_head->next = nullptr;
	}

	BuddyAllocator::pointer BuddyAllocator::allocate(size_type size, size_type alignment)
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
	BuddyAllocator::pointer BuddyAllocator::allocate(size_type size)
	{
		u64 alignment = alignof(rsl::max_align);
		return allocate(size, alignment);
	}

	void BuddyAllocator::deallocate(pointer ptr, size_type size)
	{
		rsl::byte* ptr_as_bytes = reinterpret_cast<rsl::byte*>(ptr);
		ptr_as_bytes -= sizeof(internal::BuddyBlockHeader);
		internal::BuddyBlockHeader* block = reinterpret_cast<internal::BuddyBlockHeader*>(ptr_as_bytes);

		block->next = m_head;
		m_head = block;
	}

	bool BuddyAllocator::operator==(const BuddyAllocator& rhs) const
	{
		return m_buffer.get() == rhs.m_buffer.get();
	}
	bool BuddyAllocator::operator!=(const BuddyAllocator& rhs) const
	{
		return !(*this == rhs);
	}

	BuddyAllocator::pointer BuddyAllocator::to_user_pointer(internal::BuddyBlockHeader* block) const
	{
		rsl::byte* ptr = reinterpret_cast<rsl::byte*>(block);
		return ptr += sizeof(internal::BuddyBlockHeader);
	}
	bool BuddyAllocator::can_split_block(const internal::BuddyBlockHeader* block, size_type size) const
	{
		return block->size - size > sizeof(internal::BuddyBlockHeader);
	}
	void BuddyAllocator::split_block(internal::BuddyBlockHeader* block, size_type size) const
	{
		rsl::byte* block_as_bytes = reinterpret_cast<rsl::byte*>(block);
		block_as_bytes += sizeof(internal::BuddyBlockHeader) + size;
		internal::BuddyBlockHeader* new_block = reinterpret_cast<internal::BuddyBlockHeader*>(block_as_bytes);
		new_block->size = block->size - sizeof(internal::BuddyBlockHeader) - size;
		new_block->next = block->next;

		block->size = size;
		block->next = new_block;
	}
	void BuddyAllocator::remove_free_block(internal::BuddyBlockHeader* prev, internal::BuddyBlockHeader* toRemove)
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
}