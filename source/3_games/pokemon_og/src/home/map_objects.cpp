#include <cstdint>

// ============================================================================
// DecodeArrowMovementRLE
//
// ASM behaviour:
// - HL = arrow movement table (Y, X, pointer-to-RLE)
// - B = player Y
// - C = player X
// - Searches until $FF terminator
// - On match: decodes RLE into wSimulatedJoypadStatesEnd
// ============================================================================

extern uint8_t wSimulatedJoypadStatesEnd[];
extern uint8_t wSimulatedJoypadStatesIndex;

// External routine
uint8_t DecodeRLEList(uint8_t* de, uint8_t* hl);

inline void DecodeArrowMovementRLE(uint8_t* hl, uint8_t playerY, uint8_t playerX)
{
  while (true)
  {
    uint8_t y = *hl++;

    if (y == 0xFF)
      return;

    if (y != playerY)
    {
      hl += 2; // skip X + pointer
      continue;
    }

    uint8_t x = *hl++;
    if (x != playerX)
    {
      hl += 1; // skip pointer low/high already partially consumed
      continue;
    }

    // match: de = pointer at hl
    uint8_t low = *hl++;
    uint8_t high = *hl++;
    uint8_t* de = reinterpret_cast<uint8_t*>((high << 8) | low);

    uint8_t written = DecodeRLEList(de, wSimulatedJoypadStatesEnd);

    wSimulatedJoypadStatesIndex = written - 1;
    return;
  }
}

// ============================================================================
// ArePlayerCoordsInArray / CheckCoords
// ============================================================================

extern uint8_t wYCoord;
extern uint8_t wXCoord;
extern uint8_t wCoordIndex;

inline bool CheckCoords(uint8_t* hl, uint8_t b, uint8_t c)
{
  wCoordIndex = 0;

  while (true)
  {
    uint8_t y = *hl++;

    if (y == 0xFF)
      return false;

    wCoordIndex++;

    if (y != b)
    {
      hl++; // skip X
      continue;
    }

    uint8_t x = *hl++;

    if (x != c)
      continue;

    return true;
  }
}

inline bool ArePlayerCoordsInArray(uint8_t* hl)
{
  return CheckCoords(hl, wYCoord, wXCoord);
}

// ============================================================================
// CheckBoulderCoords
// ============================================================================

extern uint8_t hSpriteIndex;
extern uint8_t wSpritePlayerStateData2MapY[];

inline bool CheckBoulderCoords(uint8_t* hl)
{
  uint8_t index = hSpriteIndex;

  uint8_t* base = wSpritePlayerStateData2MapY + (index * 16);

  uint8_t b = base[0] - 4;
  uint8_t c = base[1] - 4;

  return CheckCoords(hl, b, c);
}

// ============================================================================
// DecodeRLEList
// ============================================================================

extern uint8_t wRLEByteCount;
extern uint8_t hRLEByteValue;

inline uint8_t FillMemory(uint8_t value, uint8_t count, uint8_t* dst);

inline uint8_t DecodeRLEList(uint8_t* de, uint8_t* hl)
{
  wRLEByteCount = 0;

  while (true)
  {
    uint8_t value = *de++;

    if (value == 0xFF)
    {
      *hl = 0xFF;
      return wRLEByteCount + 1;
    }

    hRLEByteValue = value;

    uint8_t count = *de++;

    wRLEByteCount += count;

    FillMemory(value, count, hl);

    hl += count;
  }
}

// ============================================================================
// Sprite movement helpers
// ============================================================================

extern uint8_t hSpriteMovementByte2;
extern uint8_t wSpriteStateData1[];
extern uint8_t wSpriteStateData2[];
extern uint8_t hSpriteDataOffset;

// stub helpers
inline uint8_t* GetSpriteMovementByte1Pointer()
{
  uint8_t idx = hSpriteIndex;
  return wSpriteStateData1 + (idx * 16 + 6);
}

inline uint8_t* GetSpriteMovementByte2Pointer()
{
  uint8_t idx = hSpriteIndex;
  return wSpriteStateData2 + (idx * 16);
}

inline void SetSpriteMovementBytesToFE()
{
  uint8_t* p1 = GetSpriteMovementByte1Pointer();
  *p1 = 0xFE;

  uint8_t* p2 = GetSpriteMovementByte2Pointer();
  *p2 = hSpriteMovementByte2;
}

inline void SetSpriteMovementBytesToFF()
{
  uint8_t* p1 = GetSpriteMovementByte1Pointer();
  *p1 = 0xFF;

  uint8_t* p2 = GetSpriteMovementByte2Pointer();
  *p2 = 0xFF;
}