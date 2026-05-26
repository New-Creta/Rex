#include "pokemon/original_code/home/init.h"

#include "pokemon/original_code/constants/hardware_constants.h"
#include "pokemon/original_code/ram/wram.h"

#include "pokemon/original_code/home/init.h"

namespace pokemon
{
	void gbc()
	{
		wram.data.wGBC = 0;
		init();
	}
	void ok()
	{
		wram.data.wGBC = 0;
		init();
	}

	void start()
	{
		if (GBC == 0)
		{
			gbc();
		}

		ok();
	}
}


// =========================================================
// _Start
// =========================================================

#include <cstdint>

uint8_t wGBC;

constexpr uint8_t GBC = 0x01;
constexpr uint8_t FALSE = 0x00;

void Init();

void _Start(uint8_t a)
{
  if (a == GBC)
  {
    a = FALSE;
  }
  else
  {
    a = 0;
  }

  wGBC = a;

  Init();
}