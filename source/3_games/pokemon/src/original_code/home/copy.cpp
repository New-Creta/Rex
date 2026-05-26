#include <cstdint>

// ============================================================================
// FarCopyData
//
// ASM:
//   ; Copy bc bytes from a:hl to de
//   ; Switches ROM bank to A, copies data, restores bank
// ============================================================================

extern uint8_t wBuffer;
extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

inline void CopyData(uint8_t* de, const uint8_t* hl, uint16_t bc)
{
  // ASM:
  // Copy bc bytes from hl to de

  while (bc != 0)
  {
    *de++ = *hl++;
    --bc;
  }
}

// NOTE: In real GB code, hl is in switched ROM bank (A:HL)

inline void FarCopyData(
  uint8_t bankA,
  const uint8_t* hl,
  uint8_t* de,
  uint16_t bc)
{
  // ld [wBuffer], a
  wBuffer = bankA;

  // save current bank
  uint8_t savedBank = hLoadedROMBank;

  // switch bank
  hLoadedROMBank = wBuffer;
  MBC1RomBank = wBuffer;

  // copy data
  CopyData(de, hl, bc);

  // restore bank
  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;
}