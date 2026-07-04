#include <cstdint>

// Shadow OAM memory
uint8_t wShadowOAM[160];

// Writes one 2x2 sprite block into OAM
// a = oam block index (each block = 4 entries)
// b = Y coordinate (top-left)
// c = X coordinate (top-left)
// de = pointer to [tile, attribute] pairs (4 entries)
void WriteOAMBlock(uint8_t a, uint8_t b, uint8_t c, const uint8_t* de)
{
  uint8_t* hl = &wShadowOAM[(a * 16)]; // swap a * 16

  auto writeOneEntry = [&](uint8_t y, uint8_t x)
    {
      *hl++ = y;              // Y
      *hl++ = x;              // X
      *hl++ = *de++;          // tile
      *hl++ = *de++;          // attribute
    };

  // upper left
  writeOneEntry(b, c);

  // upper right
  writeOneEntry(b, c + 8);

  // lower left
  writeOneEntry(b + 8, c);

  // lower right
  writeOneEntry(b + 8, c + 8);
}