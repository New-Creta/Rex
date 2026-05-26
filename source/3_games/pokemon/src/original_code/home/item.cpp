#include <cstdint>

// ============================================================================
// UseItem
//
// ASM:
//   farjp UseItem_
//   INPUT: wcf91 = item id
//   OUTPUT: wActionResultOrTookBattleTurn
// ============================================================================

uint8_t UseItem_();

inline void UseItem()
{
  UseItem_();
}

// ============================================================================
// TossItem
//
// ASM:
//   bank switch -> TossItem_ -> restore bank
//   HL = inventory pointer
// ============================================================================

extern uint8_t hLoadedROMBank;
extern uint8_t MBC1RomBank;

void TossItem_();

inline bool TossItem()
{
  uint8_t savedBank = hLoadedROMBank;

  uint8_t bank = /* BANK(TossItem_) */ 0;
  hLoadedROMBank = bank;
  MBC1RomBank = bank;

  TossItem_();

  // pop de (restore bank)
  hLoadedROMBank = savedBank;
  MBC1RomBank = savedBank;

  // carry = success/failure not modelled directly
  return true;
}

// ============================================================================
// IsKeyItem
//
// ASM:
//   farcall IsKeyItem_
// ============================================================================

uint8_t IsKeyItem_();

inline uint8_t IsKeyItem()
{
  return IsKeyItem_();
}