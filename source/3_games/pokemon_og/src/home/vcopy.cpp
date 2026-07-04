#include <cstdint>

// Assume global memory-mapped areas / state
extern uint8_t* VRAM;
extern uint8_t* wTileMap;
extern uint8_t* sram;
extern uint8_t* sp; // stack pointer abstraction (pseudo)

constexpr int BG_MAP_WIDTH = 32;
constexpr int BG_MAP_HEIGHT = 32;
constexpr int SCREEN_WIDTH = 20;
constexpr int SCREEN_HEIGHT = 18;
constexpr int LEN_2BPP_TILE = 16;

// ---------------------------------------------
// GetRowColAddressBgMap
// ---------------------------------------------
uint16_t GetRowColAddressBgMap(uint8_t h, uint8_t l, uint8_t b)
{
  uint8_t a = 0;

  h >>= 1; a = (a >> 1);
  h >>= 1; a = (a >> 1);
  h >>= 1; a = (a >> 1);

  a |= l;

  uint16_t hl = (b << 8) | h;
  hl = (hl & 0xFF00) | a;
  return hl;
}

// ---------------------------------------------
// ClearBgMap
// ---------------------------------------------
void ClearBgMap(uint8_t* hl_base)
{
  uint8_t tile = ' ';
  uint16_t count = BG_MAP_WIDTH * BG_MAP_HEIGHT;

  uint8_t* hl = hl_base;

  for (uint16_t i = 0; i < count; i++)
  {
    *hl++ = tile;
  }
}

// ---------------------------------------------
// RedrawRowOrColumn
// ---------------------------------------------
void RedrawRowOrColumn()
{
  extern uint8_t hRedrawRowOrColumnMode;
  extern uint8_t wRedrawRowOrColumnSrcTiles[];
  extern uint16_t hRedrawRowOrColumnDest;

  if (!hRedrawRowOrColumnMode) return;

  uint8_t mode = hRedrawRowOrColumnMode;
  hRedrawRowOrColumnMode = 0;

  if (mode != 1)
  {
    // redraw column
    uint8_t* src = wRedrawRowOrColumnSrcTiles;
    uint8_t* de = (uint8_t*)hRedrawRowOrColumnDest;

    for (int i = 0; i < SCREEN_HEIGHT; i++)
    {
      *de++ = *src++;
      *de++ = *src++;

      de += BG_MAP_WIDTH - 1;

      uint8_t addr = (uint32_t)de & 0xFF;
      addr = (addr & 0x3) | 0x98;
      de = (uint8_t*)((uintptr_t)de & 0xFF00 | addr);
    }
    return;
  }

  // redraw row
  uint8_t* src = wRedrawRowOrColumnSrcTiles;
  uint8_t* de = (uint8_t*)hRedrawRowOrColumnDest;

  auto DrawHalf = [&]()
    {
      for (int i = 0; i < SCREEN_WIDTH / 2; i++)
      {
        *de++ = *src++;
        *de++ = *src++;

        uint8_t e = (uintptr_t)de & 0xFF;
        e = (e + 1) & 0x1F;
        de = (uint8_t*)((uintptr_t)de & 0xFFE0 | e);
      }
    };

  DrawHalf();
  de += BG_MAP_WIDTH;
  DrawHalf();
}

// ---------------------------------------------
// AutoBgMapTransfer
// ---------------------------------------------
void AutoBgMapTransfer()
{
  extern uint8_t hAutoBGTransferEnabled;
  if (!hAutoBGTransferEnabled) return;

  static uint8_t portion = 0;

  uint8_t* src;
  uint8_t* dest;

  if (portion == 0)
  {
    src = wTileMap;
    dest = VRAM;
    portion = 1;
  }
  else if (portion == 1)
  {
    src = wTileMap + 6 * 32;
    dest = VRAM + 6 * 32;
    portion = 2;
  }
  else
  {
    src = wTileMap + 12 * 32;
    dest = VRAM + 12 * 32;
    portion = 0;
  }

  for (int b = 0; b < 6; b++)
  {
    for (int i = 0; i < SCREEN_WIDTH - 1; i++)
    {
      *dest++ = *src++;
      *dest++ = *src++;
    }

    dest += BG_MAP_WIDTH - (SCREEN_WIDTH - 1);
  }
}

// ---------------------------------------------
// VBlankCopyBgMap
// ---------------------------------------------
void VBlankCopyBgMap()
{
  extern uint8_t hVBlankCopyBGSource;
  if (!hVBlankCopyBGSource) return;

  uint8_t* src = (uint8_t*)hVBlankCopyBGSource;
  uint8_t* dest = (uint8_t*)hVBlankCopyBGDest;
  int rows = hVBlankCopyBGNumRows;

  hVBlankCopyBGSource = 0;

  for (int r = 0; r < rows; r++)
  {
    for (int i = 0; i < SCREEN_WIDTH; i++)
    {
      *dest++ = *src++;
      *dest++ = *src++;
    }
    dest += BG_MAP_WIDTH - SCREEN_WIDTH;
  }
}

// ---------------------------------------------
// VBlankCopyDouble
// ---------------------------------------------
void VBlankCopyDouble()
{
  extern uint8_t hVBlankCopyDoubleSize;
  if (!hVBlankCopyDoubleSize) return;

  uint8_t* src = (uint8_t*)hVBlankCopyDoubleSource;
  uint8_t* dest = (uint8_t*)hVBlankCopyDoubleDest;

  int size = hVBlankCopyDoubleSize;
  hVBlankCopyDoubleSize = 0;

  for (int i = 0; i < size * (LEN_2BPP_TILE / 4); i++)
  {
    uint8_t e = *src++;
    uint8_t d = *src++;

    *dest++ = e; *dest++ = e;
    *dest++ = d; *dest++ = d;
  }
}

// ---------------------------------------------
// VBlankCopy
// ---------------------------------------------
void VBlankCopy()
{
  extern uint8_t hVBlankCopySize;
  if (!hVBlankCopySize) return;

  uint8_t* src = (uint8_t*)hVBlankCopySource;
  uint8_t* dest = (uint8_t*)hVBlankCopyDest;

  int size = hVBlankCopySize;
  hVBlankCopySize = 0;

  for (int i = 0; i < size * (LEN_2BPP_TILE / 2); i++)
  {
    uint8_t e = *src++;
    uint8_t d = *src++;

    *dest++ = e;
    *dest++ = d;
  }
}

// ---------------------------------------------
// UpdateMovingBgTiles
// ---------------------------------------------
void UpdateMovingBgTiles()
{
  extern uint8_t hTileAnimations;
  if (!hTileAnimations) return;

  static uint8_t counter1 = 0;
  static uint8_t counter2 = 0;

  counter1++;
  if (counter1 < 20) return;

  if (counter1 == 21)
  {
    // flower
    const uint8_t* tile =
      (counter2 & 3) == 0 ? FlowerTile1 :
      (counter2 & 3) == 1 ? FlowerTile2 : FlowerTile3;

    uint8_t* dest = VRAM + 0x03 * 16;

    for (int i = 0; i < 16; i++)
      *dest++ = tile[i];

    counter1 = 0;
    return;
  }

  // water animation
  uint8_t* hl = VRAM + 0x14 * 16;

  for (int i = 0; i < 16; i++)
  {
    *hl = (*hl >> 1) | (*hl << 7);
    hl++;
  }

  counter1 = 0;
}

// Flower tiles (data placeholders)
extern const uint8_t FlowerTile1[16];
extern const uint8_t FlowerTile2[16];
extern const uint8_t FlowerTile3[16];