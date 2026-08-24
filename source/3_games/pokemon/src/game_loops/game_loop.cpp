#include "pokemon/game_loops/game_loop.h"

namespace pokemon
{
	GameLoop::GameLoop(GameLoopState initialState)
		: m_state(initialState)
	{
		m_state_tree[initialState].begin();
	}

	void GameLoop::run()
	{
		GameLoopState new_state = m_state_tree[m_state].tick();
		if (new_state != m_state)
		{
			m_state_tree[m_state].end();
			m_state = new_state;
			m_state_tree[m_state].begin();
		}
	}
}