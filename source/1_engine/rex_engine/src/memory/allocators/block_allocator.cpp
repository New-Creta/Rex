#include "rex_engine/memory/allocators/block_allocator.h"

#include "rex_engine/diagnostics/assert.h"

namespace rex
{
	BlockAllocator::BlockAllocator(rsl::unique_array<rsl::byte> buffer, size_type blockSize)
		: m_buffer(rsl::move(buffer))
		, m_head()
	{
		m_head = reinterpret_cast<internal::BlockHeader*>(m_buffer.get());
		m_head->next = nullptr;
	}

	BlockAllocator::pointer BlockAllocator::allocate(size_type size, size_type alignment)
	{
		REX_UNUSED_PARAM(size); // Size cannot be used for buddy allocator
		REX_UNUSED_PARAM(alignment); // Alignment cannot be used for buddy allocator

		REX_ASSERT_X(size < m_block_size, "Trying to allocate something that's bigger than the block size of a block allocator");

		internal::BlockHeader* current = m_head;
		REX_ASSERT_X(current != nullptr, "Ran out of memory in the free list");

		remove_free_block(current);
		return to_user_pointer(current);
	}
	BlockAllocator::pointer BlockAllocator::allocate(size_type size)
	{
		u64 alignment = alignof(rsl::max_align);
		return allocate(size, alignment);
	}

	void BlockAllocator::deallocate(pointer ptr, size_type size)
	{
		rsl::byte* ptr_as_bytes = reinterpret_cast<rsl::byte*>(ptr);
		ptr_as_bytes -= sizeof(internal::BlockHeader);
		internal::BlockHeader* block = reinterpret_cast<internal::BlockHeader*>(ptr_as_bytes);

		block->next = m_head;
		m_head = block;
	}

	bool BlockAllocator::operator==(const BlockAllocator& rhs) const
	{
		return m_buffer.get() == rhs.m_buffer.get();
	}
	bool BlockAllocator::operator!=(const BlockAllocator& rhs) const
	{
		return !(*this == rhs);
	}

	BlockAllocator::pointer BlockAllocator::to_user_pointer(internal::BlockHeader* block) const
	{
		rsl::byte* ptr = reinterpret_cast<rsl::byte*>(block);
		return ptr += sizeof(internal::BlockHeader);
	}
	void BlockAllocator::remove_free_block(internal::BlockHeader* toRemove)
	{
		m_head = toRemove->next;
	}
}