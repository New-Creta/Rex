#pragma once

#include "rex_engine/engine/defines.h"
#include "rex_engine/engine/types.h"

namespace pokemon
{
	const s8 BIT_A_BUTTON = 0;
	const s8 BIT_B_BUTTON = 1;
	const s8 BIT_SELECT		= 2;
	const s8 BIT_START		= 3;
	const s8 BIT_D_RIGHT	= 4;
	const s8 BIT_D_LEFT		= 5;
	const s8 BIT_D_UP			= 6;
	const s8 BIT_D_DOWN		= 7;

	const s8 NO_INPUT = 0;
	const s8 A_BUTTON = BIT(BIT_A_BUTTON);
	const s8 B_BUTTON = BIT(BIT_B_BUTTON);
	const s8 SELECT		= BIT(BIT_SELECT);
	const s8 START		= BIT(BIT_START);
	const s8 D_RIGHT	= BIT(BIT_D_RIGHT);
	const s8 D_LEFT		= BIT(BIT_D_LEFT);
	const s8 D_UP			= BIT(BIT_D_UP);
	const s8 D_DOWN		= BIT(BIT_D_DOWN);
}