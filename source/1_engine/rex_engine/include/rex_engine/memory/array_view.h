#pragma once

#include "rex_engine/engine/types.h"

namespace rex
{
	template <typename T>
	class ArrayView
	{
	public:
		ArrayView(T* data, s32 count)
			: m_data(data)
			, m_count(count)
		{}

		T& operator[](s32 idx)
		{
			return m_data[idx];
		}
		const T& operator[](s32 idx) const
		{
			return m_data[idx];
		}

		s32 count() const
		{
			return m_count;
		}
		T* data()
		{
			return m_data;
		}
		const T* data() const
		{
			return m_data;
		}

	private:
		T* m_data;
		s32 m_count;
	};
}