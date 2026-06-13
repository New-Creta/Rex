#include "pokemon/original_code/engine/movie/intro.h"
#include "pokemon/original_code/home/fade.h"
#include "pokemon/original_code/home/clear_sprites.h"
#include "pokemon/original_code/home/vblank.h"
#include "pokemon/original_code/constants/palette_constants.h"
#include "pokemon/original_code/constants/gfx_constants.h"

#include "pokemon/original_code/ram/wram.h"
#include "pokemon/original_code/ram/hram.h"

void PlayShootingStar()
{
  RunPaletteCommand(0xc);
  Bankswitch(0x4538);
  BGP = 0xe4;
  DelayFrames(0xb4);
}

const s32 MOVE_NIDORINO_RIGHT = -1;
const s32 MOVE_GENGAR_RIGHT = 0;
const s32 MOVE_GENGAR_LEFT = 1;

void IntroMoveMon(rsl::byte NumberOfTimesToMoveMon, rsl::byte WhatToDo)
{
  byte bVar1;
  
  do {
    if (WhatToDo == MOVE_NIDORINO_RIGHT) {
      wBaseCoordX = 2;
      bVar1 = 0;
      wBaseCoordY = 0;
      UpdateIntroNidorinoOAM(0x24);
.moveGengarLeft:
      bVar1 = bVar1 & 0x1f;
      hSCX = hSCX + '\x02';
    }
    else {
      bVar1 = (WhatToDo == MOVE_GENGAR_RIGHT) << 4;
      if (WhatToDo == MOVE_GENGAR_LEFT) goto .moveGengarLeft;
      hSCX = hSCX + -2;
    }
    CheckForUserInterruption(2);
    if ((bool)(bVar1 >> 4 & 1)) {
      return;
    }
    NumberOfTimesToMoveMon = NumberOfTimesToMoveMon - 1;
    if (NumberOfTimesToMoveMon == 0) {
      return;
    }
  } while( true );
}

void PlayIntroScene(void)
{
  byte bVar1;
  undefined1 extraout_B;
  undefined1 extraout_B_00;
  undefined1 extraout_B_01;
  undefined1 extraout_B_02;
  undefined1 extraout_B_03;
  undefined1 extraout_B_04;
  undefined1 extraout_B_05;
  
  RunPaletteCommand(7);
  BGP = 0xe4;
  OBP0 = 0xe4;
  OBP1 = 0xe4;
  bVar1 = 0;
  hSCX = 0;
  IntroCopyTiles(3);
  wBaseCoordX = 0;
  wBaseCoordY = 0x50;
  InitIntroNidorinoOAM(switchD_rom28::5dfe::caseD_e);
  IntroMoveMon(0x28,MOVE_NIDORINO_RIGHT);
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // hip
  PlaySound(0xb9);
  bVar1 = 0;
  wram.data.wIntroNidorinoBaseTile = 0;
  AnimateIntroNidorino(&UNK_rom16__5910);
  
  // hop
  PlaySound(0xba);
  AnimateIntroNidorino(&UNK_rom16__591b);
  CheckForUserInterruption(CONCAT11(extraout_B,10));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // hip
  PlaySound(0xb9);
  AnimateIntroNidorino(&UNK_rom16__5910);

  // hop
  PlaySound(0xba);
  AnimateIntroNidorino(&UNK_rom16__591b);
  CheckForUserInterruption(CONCAT11(extraout_B_00,0x1e));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // raise
  IntroCopyTiles(4);
  PlaySound(0xbb);
  IntroMoveMon(4,1);
  CheckForUserInterruption(CONCAT11(extraout_B_01,0x1e));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // slash
  IntroCopyTiles(5);
  PlaySound(0xbc);
  IntroMoveMon(8,0);

  // hip
  PlaySound(0xb9);
  wram.data.wIntroNidorinoBaseTile = 0x24;
  AnimateIntroNidorino(&UNK_rom16__5926);
  CheckForUserInterruption(CONCAT11(extraout_B_02,0x1e));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }
  IntroMoveMon(4,1);
  IntroCopyTiles(3);
  CheckForUserInterruption(CONCAT11(extraout_B_03,0x3c));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // hip
  PlaySound(0xb9);
  bVar1 = 0;
  wram.data.wIntroNidorinoBaseTile = 0;
  AnimateIntroNidorino(&UNK_rom16__5931);
  
  // hop
  PlaySound(0xba);
  AnimateIntroNidorino(&UNK_rom16__593c);
  CheckForUserInterruption(CONCAT11(extraout_B_04,0x14));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }
  wram.data.wIntroNidorinoBaseTile = 0x24;
  AnimateIntroNidorino(&UNK_rom16__5947);
  CheckForUserInterruption(CONCAT11(extraout_B_05,0x1e));
  if ((bool)(bVar1 >> 4 & 1)) {
    return;
  }

  // lunge
  PlaySound(0xb8);
  wram.data.wIntroNidorinoBaseTile = 0x48;
  AnimateIntroNidorino(&UNK_rom16__5950);
  return;
}

void PlayIntro()
{
  hram.hJoyHeld = 0;
  hram.hAutoBGTransferEnabled = 1;
  PlayShootingStar();
  PlayIntroScene();
  GBFadeOutToWhite();
  hram.hSCX = 0;
  hram.hAutoBGTransferEnabled = 0;
  ClearSprites();
  DelayFrame();
}