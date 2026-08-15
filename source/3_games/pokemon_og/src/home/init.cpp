#include "pokemon/original_code/home/init.h"

#include "pokemon/original_code/home/lcd.h"
#include "pokemon/original_code/ram/wram.h"
#include "pokemon/original_code/engine/movie/intro.h"

void Init(void)
{
  wram.audio_data.wAudioROMBank       = 0x1f;
  wram.audio_data.wAudioSavedROMBank  = 0x1f;
  wram.data.wUpdateSpritesEnabled     = 0xff;
  PlayIntro();

  DisableLCD();
  ClearVram();
  GBPalNormal();
  ClearSprites();
  LCDC = 0xe3;

  PrepareTitleScreen();
}