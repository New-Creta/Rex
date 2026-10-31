#pragma once

#include "pokemon/state_task.h"

namespace pokemon
{
	class CopyrightState : public StateTask
	{
	public:
		CopyrightState();

		void begin();
		GameState tick(s32 frameIdx);
		void end();

	private:

	};
}