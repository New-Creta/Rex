#pragma once

namespace rex
{
	//-------------------------------------------------------------------------
	constexpr size_t hash(const int8* keyName)
	{
		const uint32 p = 31;
		const uint32 m = (uint32)1e9 + 9;

		long long hash_value = 0;
		long long p_pow = 1;
		const char* c = keyName;

		while (*c)
		{
			hash_value = (hash_value + (*c - (size_t)'a' + 1) * p_pow) % m;
			p_pow = (p_pow * p) % m;
			c++;
		}

		return (size_t)hash_value;
	}
}