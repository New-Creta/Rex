// ===============================
// Math / Movement routines
// ===============================

#include <cstdint>
#include <cstdlib>

// externs
extern uint8_t wNPCMovementDirections[256];
extern uint8_t wNPCNumScriptedSteps;
extern uint8_t wOverrideSimulatedJoypadStatesMask;
extern uint8_t wSimulatedJoypadStatesEnd;
extern uint8_t wJoyIgnore;
extern uint8_t wUnusedCD3A;
extern uint8_t wd730;
extern uint8_t hSpriteIndex;
extern uint8_t hDividend2;
extern uint8_t hDivisor2;
extern uint8_t hQuotient2[2];

// external helpers
extern void SetSpriteMovementBytesToFF();
extern uint8_t* GetSpriteMovementByte1Pointer();

// ===============================
// CalcDifference
// returns |a-b|, sets carry if a < b
// ===============================
uint8_t CalcDifference(uint8_t a, uint8_t b, bool& carry)
{
  int16_t result = (int16_t)a - (int16_t)b;

  if (result >= 0)
  {
    carry = false;
    return (uint8_t)result;
  }
  else
  {
    carry = true;
    return (uint8_t)(-result);
  }
}

// ===============================
// MoveSprite
// ===============================
void MoveSprite(uint8_t* de)
{
  SetSpriteMovementBytesToFF();

  MoveSprite_(de);
}

void MoveSprite_(uint8_t* de)
{
  GetSpriteMovementByte1Pointer()[0] = 0;

  uint8_t c = 0;
  uint8_t* hl = wNPCMovementDirections;

  while (true)
  {
    uint8_t v = *de++;
    *hl++ = v;
    c++;

    if ((int8_t)v == -1)
      break;
  }

  wNPCNumScriptedSteps = c;

  wd730 |= (1 << 0);

  wOverrideSimulatedJoypadStatesMask = 0;
  wSimulatedJoypadStatesEnd = 0;
  wJoyIgnore = 0xFF;
  wUnusedCD3A = 0;
}

// ===============================
// DivideBytes
// quotient = hDividend2 / hDivisor2
// ===============================
void DivideBytes()
{
  uint8_t* q = hQuotient2;

  q[1] = 0;

  uint8_t dividend = hDividend2;
  uint8_t divisor = hDivisor2;

  if (divisor == 0)
    return;

  uint8_t quotient = 0;

  while (dividend >= divisor)
  {
    dividend -= divisor;
    quotient++;
  }

  q[0] = quotient;
}