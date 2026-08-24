#pragma once

#include "rex_std/memory.h"
#include "rex_std/unordered_map.h"

#include "pokemon/state_task.h"
#include "pokemon/game_loops/game_loop_state.h"

namespace pokemon
{
	class GameLoop
	{
	public:
		GameLoop(GameLoopState initialState);
		void run();

	private:
		GameLoopState m_state;
		rsl::unordered_map<GameLoopState, StateTask> m_state_tree;
	};
}