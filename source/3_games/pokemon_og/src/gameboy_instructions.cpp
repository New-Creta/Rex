#include "pokemon/original_code/gameboy_instructions.h"

namespace pokemon
{
	void gameboy_di()
	{

	}

	s16 HIGH(s32 n)
	{
		return (n & 0xFF00) >> 8;
	}
	s8 LOW(s32 n)
	{
		n & 0xFF;
	}
}