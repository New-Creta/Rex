#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class OverworldState : public StateTask
	{
	public:
		OverworldState();

		void begin();
		GameLoopState tick(s32 frameIdx);
		void end();

	private:

	};
}