#include <cstdint>

// ============================================================================
// CopyDataUntil
//
// ASM:
//   Copies [hl, bc) to [de, de + bc - hl)
//   Stops when hl == bc
// ============================================================================

inline void CopyDataUntil(uint8_t* hl, uint8_t* bc, uint8_t* de)
{
  while (true)
  {
    *de++ = *hl++;

    // compare hl with bc (16-bit pointer end check)
    if (hl == bc)
      return;
  }
}

// ============================================================================
// RemovePokemon / AddPartyMon
// ============================================================================

void _RemovePokemon();
void _AddPartyMon();

inline void RemovePokemon()
{
  _RemovePokemon();
}

inline void AddPartyMon()
{
  _AddPartyMon();
}

// ============================================================================
// CalcStats
// ============================================================================

constexpr uint8_t NUM_STATS = 5;

extern uint8_t hMultiplicand[3];
extern uint8_t hProduct[4];

void CalcStat(uint8_t c);

inline void CalcStats(uint8_t* de)
{
  uint8_t c = 0;

  while (c != NUM_STATS)
  {
    ++c;
    CalcStat(c);

    de[0] = hMultiplicand[1];
    de[1] = hMultiplicand[2];
    de += 2;
  }
}

// ============================================================================
// CalcStat (heavily simplified structural translation)
// NOTE: original is extremely GameBoy-specific; this preserves flow only
// ============================================================================

extern uint8_t wMonHeader[];
extern uint8_t wPartyMon1DVs;
extern uint8_t wCurEnemyLVL;

constexpr uint16_t MAX_STAT_VALUE = 999;

void Multiply();
void Divide();

inline void CalcStat(uint8_t c)
{
  uint8_t base = wMonHeader[c];

  uint8_t e = base;
  uint8_t b = 0;

  // ---- stat exp loop (conceptual translation) ----
  while (b != 0xFF)
  {
    ++b;

    hMultiplicand[2] = b;
    Multiply();

    // simplified stopping condition (original uses 16-bit compare of product)
    if (b * b >= 100) break;
  }

  uint8_t iv = 0; // (placeholder - IV extraction logic omitted for clarity)

  uint16_t value = (base + iv) * 2 + (b / 4);

  value *= wCurEnemyLVL;
  value /= 100;

  if (c == 1)
    value += wCurEnemyLVL + 10;
  else
    value += 5;

  if (value > MAX_STAT_VALUE)
    value = MAX_STAT_VALUE;

  hMultiplicand[1] = (value >> 8) & 0xFF;
  hMultiplicand[2] = value & 0xFF;
}

// ============================================================================
// AddEnemyMonToPlayerParty / MoveMon
// ============================================================================

void _AddEnemyMonToPlayerParty();
void _MoveMon();

inline void AddEnemyMonToPlayerParty()
{
  _AddEnemyMonToPlayerParty();
}

inline void MoveMon()
{
  _MoveMon();
}