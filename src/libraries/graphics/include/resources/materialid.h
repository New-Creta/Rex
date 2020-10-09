#pragma once

namespace rex
{
	namespace graphics
	{
		struct MaterialID
		{
			//-------------------------------------------------------------------------
			MaterialID(const std::wstring& path)
				: m_id(std::hash<std::wstring>{}(path))
			{}

			//-------------------------------------------------------------------------
			bool operator<(const MaterialID& id) const
			{
				return m_id < id.m_id;
			}
			//-------------------------------------------------------------------------
			bool operator>(const MaterialID& id) const
			{
				return m_id > id.m_id;
			}

			//-------------------------------------------------------------------------
			bool operator==(const MaterialID& id) const
			{
				return m_id == id.m_id;
			}
			//-------------------------------------------------------------------------
			bool operator!=(const MaterialID& id) const
			{
				return m_id != id.m_id;
			}

			size_t m_id;
		};
	}
}