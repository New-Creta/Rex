// ==========================================
// Mon Sprite Decompression / Rendering
// C++ translation (engine-style pseudocode)
// ==========================================

#include <cstdint>

// --------------------------
// external state / memory
// --------------------------
extern uint8_t wMonHeader[16];
extern uint8_t wSpriteInputPtr[2];
extern uint8_t wcf91;

extern uint8_t wMonHFrontSprite;
extern uint8_t wMonHSpriteDim;

extern uint8_t hSpriteWidth;
extern uint8_t hSpriteHeight;
extern uint8_t hSpriteOffset;
extern uint8_t hSpriteInterlaceCounter;

extern uint8_t wSpriteFlipped;

extern uint8_t hLoadedROMBank;

// VRAM / SRAM sprite buffers
extern uint8_t sSpriteBuffer0[0xC4];
extern uint8_t sSpriteBuffer1[0xC4];
extern uint8_t sSpriteBuffer2[0xC4];

// SRAM bank register (mock)
extern uint8_t MBC1SRamBank;

// --------------------------
// constants / helpers
// --------------------------
constexpr int SPRITEBUFFERSIZE = 0xC4;

// species constants
constexpr uint8_t MEW = 0x15;
constexpr uint8_t FOSSIL_KABUTOPS = 0x2F;
constexpr uint8_t TANGELA = 0x43;
constexpr uint8_t MOLTRES = 0x92;
constexpr uint8_t BEEDRILL = 0x1E;
constexpr uint8_t STARMIE = 0x59;

// external engine calls
extern void CopyVideoData(uint8_t bank, uint8_t count);
extern void ZeroSpriteBuffer(uint8_t* buf, int size);
extern void AlignSpriteDataCentered(uint8_t* de, uint8_t* hl);
extern void InterlaceMergeSpriteBuffers(uint8_t* de);
extern void SetSRAMBank(uint8_t bank);

// bank macros (placeholder)
uint8_t BANK(int) { return 0; }

// =====================================================
// UncompressMonSprite
// =====================================================
void UncompressMonSprite(uint8_t* hlOffset)
{
  uint8_t* mon = wMonHeader;

  uint8_t* ptr = hlOffset + (uintptr_t)mon;

  wSpriteInputPtr[0] = ptr[0];
  wSpriteInputPtr[1] = ptr[1];

  uint8_t id = wcf91;
  uint8_t bank;

  if (id == MEW)
    bank = BANK(/*MewPicFront*/0);
  else if (id == FOSSIL_KABUTOPS)
    bank = BANK(/*FossilKabutopsPic*/0);
  else if (id < TANGELA + 1)
    bank = BANK("Pics 1");
  else if (id < MOLTRES + 1)
    bank = BANK("Pics 2");
  else if (id < BEEDRILL + 2)
    bank = BANK("Pics 3");
  else if (id < STARMIE + 1)
    bank = BANK("Pics 4");
  else
    bank = BANK("Pics 5");

  UncompressSpriteData(bank);
}

// =====================================================
// LoadMonFrontSprite
// =====================================================
void LoadMonFrontSprite(uint8_t* de)
{
  uint8_t* hl = (uint8_t*)&wMonHFrontSprite;

  UncompressMonSprite(hl);

  uint8_t dim = wMonHSpriteDim;

  LoadUncompressedSpriteData(de, dim);
}

// =====================================================
// LoadUncompressedSpriteData
// =====================================================
void LoadUncompressedSpriteData(uint8_t* de, uint8_t dim)
{
  uint8_t w = dim & 0x0F;

  hSpriteWidth = w;

  uint8_t offsetX = (8 - w) / 2;
  hSpriteOffset = (7 * offsetX);

  uint8_t h = (dim >> 4) & 0x0F;

  hSpriteHeight = h * 8;

  uint8_t offsetY = 7 - h;

  hSpriteOffset += (7 * offsetY) * 8;

  SetSRAMBank(0);

  ZeroSpriteBuffer(sSpriteBuffer0, SPRITEBUFFERSIZE);

  AlignSpriteDataCentered(de, sSpriteBuffer0);

  ZeroSpriteBuffer(sSpriteBuffer1, SPRITEBUFFERSIZE);

  AlignSpriteDataCentered(de, sSpriteBuffer1);

  InterlaceMergeSpriteBuffers(de);
}

// =====================================================
// AlignSpriteDataCentered
// =====================================================
void AlignSpriteDataCentered(uint8_t* de, uint8_t* hl)
{
  uint8_t offset = hSpriteOffset;

  hl += offset;

  uint8_t width = hSpriteWidth;

  for (uint8_t x = 0; x < width; x++)
  {
    uint8_t height = hSpriteHeight;

    for (uint8_t y = 0; y < height; y++)
    {
      *hl++ = *de++;
    }

    hl += 7 * 8;
  }
}

// =====================================================
// ZeroSpriteBuffer
// =====================================================
void ZeroSpriteBuffer(uint8_t* buf, int size)
{
  for (int i = 0; i < size; i++)
    buf[i] = 0;
}

// =====================================================
// InterlaceMergeSpriteBuffers
// =====================================================
void InterlaceMergeSpriteBuffers(uint8_t* de)
{
  SetSRAMBank(0);

  uint8_t* dst = sSpriteBuffer2 + SPRITEBUFFERSIZE - 1;
  uint8_t* src1 = sSpriteBuffer1 + SPRITEBUFFERSIZE - 1;
  uint8_t* src0 = sSpriteBuffer0 + SPRITEBUFFERSIZE - 1;

  int count = SPRITEBUFFERSIZE / 2;

  while (count--)
  {
    *dst-- = *src1--;
    *dst-- = *src0--;
    *dst-- = *src1--;
    *dst-- = *src0--;
  }

  if (wSpriteFlipped)
  {
    for (int i = 0; i < 2 * SPRITEBUFFERSIZE; i++)
    {
      sSpriteBuffer1[i] = (sSpriteBuffer1[i] >> 4) | (sSpriteBuffer1[i] << 4);
    }
  }

  CopyVideoData(hLoadedROMBank, (2 * SPRITEBUFFERSIZE) / 16);
}