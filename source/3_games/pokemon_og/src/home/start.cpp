#include "pokemon/original_code/home/init.h"
#include "pokemon/original_code/ram/wram.h"

#include "rex_std/memory.h"

void _Start(rsl::byte a)
{
	wram.data.wGBC = 0;
	Init();
}