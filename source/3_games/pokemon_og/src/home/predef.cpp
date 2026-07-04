#include <cstdint>

using u8 = uint8_t;
using u16 = uint16_t;

// ------------------------------------------------------------
// Global RAM-equivalents
// ------------------------------------------------------------
u8 wPredefID;
u8 wPredefParentBank;
u8 wPredefBank;

u8 wPredefHL[2];
u8 wPredefDE[2];
u8 wPredefBC[2];

u8 hLoadedROMBank;

// MBC switch stub
void SetROMBank(u8 bank)
{
  // hardware bank switch emulation
  (void)bank;
}

// Predef pointer resolver (returns function pointer in hl equivalent)
using PredefFunc = void(*)();

PredefFunc GetPredefPointer();

// ------------------------------------------------------------
// Predef
// ------------------------------------------------------------
void Predef(u8 a)
{
  // Save predef id
  wPredefID = a;

  // Save parent bank (hack for warp / tileset system)
  wPredefParentBank = hLoadedROMBank;

  u8 savedBank = hLoadedROMBank;

  SetROMBank(BANK_GetPredefPointer);

  PredefFunc hl = GetPredefPointer();

  SetROMBank(wPredefBank);

  hl(); // jp hl equivalent

  SetROMBank(savedBank);
}

// ------------------------------------------------------------
// GetPredefRegisters
// ------------------------------------------------------------
void GetPredefRegisters()
{
  u16 hl = (u16(wPredefHL[1]) << 8) | wPredefHL[0];
  u16 de = (u16(wPredefDE[1]) << 8) | wPredefDE[0];
  u16 bc = (u16(wPredefBC[1]) << 8) | wPredefBC[0];

  (void)hl;
  (void)de;
  (void)bc;
}