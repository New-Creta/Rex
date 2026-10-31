#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class OverworldState : public StateTask
	{
	public:
		OverworldState();

		void begin();
		GameState tick(s32 frameIdx);
		void end();

	private:

	};
}