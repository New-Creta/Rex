#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

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
	class BlockAllocator
	{
	public:
		using size_type = s32;
		using pointer = void*;

		BlockAllocator(rsl::unique_array<rsl::byte> buffer, size_type blockSize);

		REX_NO_DISCARD pointer allocate(size_type size, size_type alignment);
		REX_NO_DISCARD pointer allocate(size_type size);
		template <typename T>
		REX_NO_DISCARD T* allocate()
		{
			return static_cast<T*>(allocate(sizeof(T), alignof(T)));
		}

		void deallocate(pointer ptr, size_type size);
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

		bool operator==(const BlockAllocator& rhs) const;
		bool operator!=(const BlockAllocator& rhs) const;

	private:
		pointer to_user_pointer(internal::BlockHeader* block) const;
		void remove_free_block(internal::BlockHeader* toRemove);

	private:
		rsl::unique_array<rsl::byte> m_buffer;
		size_type m_block_size;
		internal::BlockHeader* m_head;
	};
}