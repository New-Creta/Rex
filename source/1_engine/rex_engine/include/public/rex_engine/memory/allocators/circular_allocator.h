#pragma once

#include "rex_engine/engine/types.h"
#include "rex_engine/engine/defines.h"

#include "rex_std/memory.h"

namespace rex
{
	// Ring buffer implementation
	class CircularAllocator
	{
	public:
		using size_type = s32;
		using pointer = void*;

		CircularAllocator(rsl::unique_array<rsl::byte> buffer);

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

		bool operator==(const CircularAllocator& rhs) const;
		bool operator!=(const CircularAllocator& rhs) const;

	private:
		rsl::unique_array<rsl::byte> m_buffer;
		rsl::byte* m_current;
		const rsl::byte* m_end;
	};
}