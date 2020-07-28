#pragma once

namespace rex
{
	namespace utility
	{
		class YesNoEnum
		{
		public:
			enum Type
			{
				NO,
				YES
			};

			YesNoEnum(Type value) :
				m_value(value)
			{}

			operator Type() const
			{
				return m_value;
			}

			operator bool() const
			{
				return m_value == Type::YES;
			}

			std::string toString() const
			{
				if (m_value == Type::YES)
				{
					return "Yes";
				}
				
				return "No";
			}

		private:
			Type m_value;
		};
	}
}