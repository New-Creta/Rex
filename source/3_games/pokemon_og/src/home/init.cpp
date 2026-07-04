#include "pokemon/original_code/home/init.h"

#include "pokemon/original_code/home/lcd.h"
#include "pokemon/original_code/engine/movie/intro.h"

void Init(void)
{
  wAudioROMBank = 0x1f;
  wAudioSavedROMBank = 0x1f;
  wUpdateSpritesEnabled = 0xff;
  PlayIntro();

  DisableLCD();
  ClearVram();
  GBPalNormal();
  ClearSprites();
  LCDC = 0xe3;

  PrepareTitleScreen();
}