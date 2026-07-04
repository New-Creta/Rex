#include <cstdint>

// ============================================================================
// HasEnoughMoney
//
// ASM:
//   Compares 3-byte BCD values:
//     wPlayerMoney (de) vs hMoney (hl)
//   Uses StringCmp (big-endian compare style)
//   Returns via flags (carry/zero semantics in ASM)
//
// In C++: return true if playerMoney >= hMoney
// ============================================================================

extern uint8_t wPlayerMoney[3];
extern uint8_t hMoney[3];

// returns true if wPlayerMoney >= hMoney
inline bool HasEnoughMoney()
{
  for (int i = 0; i < 3; ++i)
  {
    uint8_t player = wPlayerMoney[i];
    uint8_t cost = hMoney[i];

    if (player > cost) return true;
    if (player < cost) return false;
  }
  return true; // equal
}

// ============================================================================
// HasEnoughCoins
//
// ASM:
//   Compares 2-byte BCD values:
//     wPlayerCoins (de) vs hCoins (hl)
// ============================================================================

extern uint8_t wPlayerCoins[2];
extern uint8_t hCoins[2];

inline bool HasEnoughCoins()
{
  for (int i = 0; i < 2; ++i)
  {
    uint8_t player = wPlayerCoins[i];
    uint8_t cost = hCoins[i];

    if (player > cost) return true;
    if (player < cost) return false;
  }
  return true;
}