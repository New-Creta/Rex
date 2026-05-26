#include <cstdint>

// ============================================================================
// GB palette fade system
//
// ASM summary:
//   - Loads 3 hardware palettes:
//       rBGP, rOBP0, rOBP1
//   - Uses precomputed fade tables (FadePal1..8)
//   - Performs stepped fading with frame delays
// ============================================================================

extern uint8_t wMapPalOffset;

extern uint8_t rBGP;
extern uint8_t rOBP0;
extern uint8_t rOBP1;

// Fake palette data representation (flattened triplets per step)
extern const uint8_t FadePal1[];
extern const uint8_t FadePal4[];
extern const uint8_t FadePal6[];
extern const uint8_t FadePal7[];

// Delay helper
void DelayFrames(uint8_t c);

// ============================================================================
// LoadGBPal
// ============================================================================

inline void LoadGBPal()
{
  uint8_t offset = wMapPalOffset;

  // hl = FadePal4 - offset (word table arithmetic)
  const uint8_t* base = FadePal4;
  const uint8_t* hl = base - offset;

  rBGP = hl[0];
  rOBP0 = hl[1];
  rOBP1 = hl[2];
}

// ============================================================================
// GBFadeInFromBlack / GBFadeOutToWhite
// ============================================================================

inline void GBFadeIncCommon(const uint8_t* hl, uint8_t b)
{
  while (b--)
  {
    rBGP = hl[0];
    rOBP0 = hl[1];
    rOBP1 = hl[2];

    hl += 3;

    DelayFrames(8);
  }
}

inline void GBFadeInFromBlack()
{
  GBFadeIncCommon(FadePal1, 4);
}

inline void GBFadeOutToWhite()
{
  GBFadeIncCommon(FadePal6, 3);
}

// ============================================================================
// GBFadeOutToBlack / GBFadeInFromWhite
// ============================================================================

inline void GBFadeDecCommon(const uint8_t* hl, uint8_t b)
{
  // ASM uses hld (reverse traversal)
  const uint8_t* p = hl;

  while (b--)
  {
    rOBP1 = *(--p);
    rOBP0 = *(--p);
    rBGP = *(--p);

    DelayFrames(8);
  }
}

inline void GBFadeOutToBlack()
{
  GBFadeDecCommon(FadePal4 + 2, 4);
}

inline void GBFadeInFromWhite()
{
  GBFadeDecCommon(FadePal7 + 2, 3);
}

// ============================================================================
// Fade tables (Game Boy palette triplets)
// Each step: [BGP, OBP0, OBP1]
// ============================================================================

const uint8_t FadePal1[] =
{
    3,3,3, 3,3,3, 3,3,3, 3,3,3
};

const uint8_t FadePal2[] =
{
    3,3,3, 2, 3,3,3, 2, 3,3,2,0
};

const uint8_t FadePal3[] =
{
    3,3,2,1, 3,2,1,0, 3,2,1,0
};

const uint8_t FadePal4[] =
{
    3,2,1,0, 3,1,0,0, 3,2,0,0
};

const uint8_t FadePal5[] =
{
    3,2,1,0, 3,1,0,0, 3,2,0,0
};

const uint8_t FadePal6[] =
{
    2,1,0,0, 2,0,0,0, 2,1,0,0
};

const uint8_t FadePal7[] =
{
    1,0,0,0, 1,0,0,0, 1,0,0,0
};

const uint8_t FadePal8[] =
{
    0,0,0,0, 0,0,0,0, 0,0,0,0
};