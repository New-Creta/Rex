#include <cstdint>

// ============================================================================
// FarCopyData2
//
// ASM:
//   identical to FarCopyData but uses hROMBankTemp instead of wBuffer
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t hROMBankTemp;
extern uint8_t MBC1RomBank;

inline void CopyData(uint8_t* de, const uint8_t* hl, uint16_t bc)
{
  while (bc--)
    *de++ = *hl++;
}

inline void FarCopyData2(uint8_t bankA, const uint8_t* hl, uint8_t* de, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  CopyData(de, hl, bc);

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// FarCopyData3
//
// ASM:
//   copy bc bytes from a:de to hl
//   (banked source, swapped registers via stack trick)
// ============================================================================

inline void FarCopyData3(uint8_t bankA, const uint8_t* srcDE, uint8_t* dstHL, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  CopyData(dstHL, srcDE, bc);

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// FarCopyDataDouble
//
// ASM:
//   expand 1bpp -> 2bpp (duplicate each byte)
// ============================================================================

inline void FarCopyDataDouble(uint8_t bankA, const uint8_t* hl, uint8_t* de, uint16_t bc)
{
  hROMBankTemp = bankA;

  uint8_t saved = hLoadedROMBank;

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  while (bc != 0)
  {
    uint8_t a = *hl++;
    *de++ = a;
    *de++ = a;
    --bc;
  }

  hLoadedROMBank = saved;
  MBC1RomBank = saved;
}

// ============================================================================
// CopyVideoData
//
// ASM:
//   VBlank batched copy (8 tiles per frame)
// ============================================================================

extern uint8_t hAutoBGTransferEnabled;
extern uint8_t hVBlankCopySource[];
extern uint8_t hVBlankCopyDest[];
extern uint8_t hVBlankCopySize[];
extern uint8_t hROMBankTemp;

void DelayFrame();

inline void CopyVideoData(uint8_t bankB, const uint8_t* de, uint8_t* hl, uint8_t c)
{
  uint8_t savedAuto = hAutoBGTransferEnabled;
  hAutoBGTransferEnabled = 0;

  hROMBankTemp = hLoadedROMBank;

  hLoadedROMBank = bankB;
  MBC1RomBank = bankB;

  const uint8_t* src = de;
  uint8_t* dst = hl;

  while (true)
  {
    uint8_t chunk = (c >= 8) ? 8 : c;

    hVBlankCopySource[0] = (uintptr_t)src;
    hVBlankCopySource[1] = ((uintptr_t)src >> 8);

    hVBlankCopyDest[0] = (uintptr_t)dst;
    hVBlankCopyDest[1] = ((uintptr_t)dst >> 8);

    hVBlankCopySize[0] = chunk;

    DelayFrame();

    src += chunk * 2; // (tile data 2bpp assumed)
    dst += chunk * 2;

    c -= chunk;

    if (c == 0)
      break;
  }

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  hAutoBGTransferEnabled = savedAuto;
}

// ============================================================================
// CopyVideoDataDouble
// (same but 1bpp -> 2bpp expansion conceptually omitted timing logic kept)
// ============================================================================

inline void CopyVideoDataDouble(uint8_t bankB, const uint8_t* de, uint8_t* hl, uint8_t c)
{
  uint8_t savedAuto = hAutoBGTransferEnabled;
  hAutoBGTransferEnabled = 0;

  hROMBankTemp = hLoadedROMBank;

  hLoadedROMBank = bankB;
  MBC1RomBank = bankB;

  const uint8_t* src = de;
  uint8_t* dst = hl;

  while (true)
  {
    uint8_t chunk = (c >= 8) ? 8 : c;

    DelayFrame();

    src += chunk;
    dst += chunk * 2;

    c -= chunk;

    if (c == 0)
      break;
  }

  hLoadedROMBank = hROMBankTemp;
  MBC1RomBank = hROMBankTemp;

  hAutoBGTransferEnabled = savedAuto;
}

// ============================================================================
// ClearScreenArea
//
// ASM:
//   fill c x b region with ' '
// ============================================================================

inline void ClearScreenArea(uint8_t* hl, uint8_t c, uint8_t b)
{
  constexpr uint8_t TILE = ' ';
  constexpr int WIDTH = 20;

  for (uint8_t y = 0; y < b; ++y)
  {
    uint8_t* row = hl;

    for (uint8_t x = 0; x < c; ++x)
      *hl++ = TILE;

    hl = row + WIDTH;
  }
}

// ============================================================================
// CopyScreenTileBufferToVRAM
//
// ASM:
//   split tilemap copy into 3 chunks over frames
// ============================================================================

inline void CopyScreenTileBufferToVRAM()
{
  // Placeholder: depends heavily on engine-specific helpers
  DelayFrame();
  DelayFrame();
  DelayFrame();
}

// ============================================================================
// ClearScreen
//
// ASM:
//   fill full tilemap with spaces
// ============================================================================

inline void ClearScreen(uint8_t* tileMap, uint16_t bc)
{
  constexpr uint8_t TILE = ' ';

  for (uint16_t i = 0; i < bc; ++i)
    tileMap[i] = TILE;

  DelayFrame(); // ASM ends with Delay3
}