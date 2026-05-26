#include <cstdint>
#include <cstring>

uint8_t wSpriteInputPtr[2];
uint8_t wTileMapBackup[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t wTileMapBackup2[SCREEN_WIDTH * SCREEN_HEIGHT];
uint8_t hAutoBGTransferEnabled;

uint8_t* coord0_0(); // hlcoord 0,0
uint8_t* decoord0_0(); // decoord 0,0

void UncompressSpriteData();
void CopyData(void* dst, const void* src, uint16_t size);

// ------------------------------------------------------------

void FillMemory(uint8_t* hl, uint16_t bc, uint8_t a)
{
  uint8_t d = a;

  while (bc != 0)
  {
    *hl++ = d;
    bc--;
  }
}

// ------------------------------------------------------------

void UncompressSpriteFromDE(uint8_t d, uint8_t e)
{
  wSpriteInputPtr[0] = e;
  wSpriteInputPtr[1] = d;
  UncompressSpriteData();
}

// ------------------------------------------------------------

void SaveScreenTilesToBuffer2()
{
  uint8_t* hl = coord0_0();
  std::memcpy(wTileMapBackup2, hl, SCREEN_WIDTH * SCREEN_HEIGHT);
}

// ------------------------------------------------------------

void LoadScreenTilesFromBuffer2()
{
  LoadScreenTilesFromBuffer2DisableBGTransfer();
  hAutoBGTransferEnabled = 1;
}

// ------------------------------------------------------------

void LoadScreenTilesFromBuffer2DisableBGTransfer()
{
  hAutoBGTransferEnabled = 0;

  uint8_t* hl = wTileMapBackup2;
  uint8_t* de = decoord0_0();

  std::memcpy(de, hl, SCREEN_WIDTH * SCREEN_HEIGHT);
}

// ------------------------------------------------------------

void SaveScreenTilesToBuffer1()
{
  uint8_t* hl = coord0_0();
  std::memcpy(wTileMapBackup, hl, SCREEN_WIDTH * SCREEN_HEIGHT);
}

// ------------------------------------------------------------

void LoadScreenTilesFromBuffer1()
{
  hAutoBGTransferEnabled = 0;

  uint8_t* hl = wTileMapBackup;
  uint8_t* de = decoord0_0();

  std::memcpy(de, hl, SCREEN_WIDTH * SCREEN_HEIGHT);

  hAutoBGTransferEnabled = 1;
}