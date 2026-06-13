#include "pokemon/original_code/home/lcd.h"

void DisableLCD(void)
{
  byte bVar1;
  byte bVar2;

  IF = 0;
  bVar2 = IE;
  IE = bVar2 & 0xfe;
  do {
    bVar1 = LY;
  } while (bVar1 != 0x91);
  bVar1 = LCDC;
  LCDC = bVar1 & 0x7f;
  IE = bVar2;
  return;
}