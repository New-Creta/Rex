#pragma once

#include "rex_std/memory.h"

namespace pokemon
{
	class GameLoop;
	class GameLoop
	{
	public:
		virtual rsl::unique_ptr<GameLoop> run() = 0;

	protected:


	private:

	};
}