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
	class BuddyAllocator
	{
	public:
		using size_type = s32;
		using pointer = void*;

		BuddyAllocator(rsl::unique_array<rsl::byte> buffer);

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

		bool operator==(const BuddyAllocator& rhs) const;
		bool operator!=(const BuddyAllocator& rhs) const;

	private:
		pointer to_user_pointer(internal::BuddyBlockHeader* block) const;
		bool can_split_block(const internal::BuddyBlockHeader* block, size_type size) const;
		void split_block(internal::BuddyBlockHeader* block, size_type size) const;
		void remove_free_block(internal::BuddyBlockHeader* prev, internal::BuddyBlockHeader* toRemove);

	private:
		rsl::unique_array<rsl::byte> m_buffer;
		internal::BuddyBlockHeader* m_head;
	};
}