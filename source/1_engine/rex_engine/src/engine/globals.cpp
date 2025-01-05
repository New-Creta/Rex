#include "rex_engine/engine/globals.h"
#include "rex_engine/engine/mutable_globals.h"

namespace rex
{
	Globals g_globals{};

	const Globals& globals()
	{
		return g_globals;
	}

	Globals& mut_globals()
	{
		return g_globals;
	}
}
