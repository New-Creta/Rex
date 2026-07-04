#include <cstdint>

// ============================================================================
// JoypadLowSensitivity
//
// ASM summary:
//  Mode 1: hJoy7 == 0 -> use pressed-only input
//  Mode 2: hJoy7 == 1, hJoy6 != 0 -> auto-repeat with delay
//  Mode 3: hJoy7 == 1, hJoy6 == 0 -> same as 2 but suppress if A/B held
// ============================================================================

extern uint8_t hJoy5;
extern uint8_t hJoy6;
extern uint8_t hJoy7;

extern uint8_t hJoyPressed;
extern uint8_t hJoyHeld;
extern uint8_t hFrameCounter;

constexpr uint8_t A_BUTTON = 0x01;
constexpr uint8_t B_BUTTON = 0x02;

void Joypad();

inline void JoypadLowSensitivity()
{
  Joypad();

  uint8_t a;

  // mode select: hJoy7
  if (hJoy7 == 0)
    a = hJoyPressed;
  else
    a = hJoyHeld;

  hJoy5 = a;

  if (hJoyPressed != 0)
  {
    hFrameCounter = 30;
    return;
  }

  if (hFrameCounter != 0)
  {
    --hFrameCounter;
    hJoy5 = 0;
    return;
  }

  uint8_t held = hJoyHeld;

  // if A or B held
  if ((held & (A_BUTTON | B_BUTTON)) != 0)
  {
    if (hJoy6 == 0)
    {
      hJoy5 = 0;
    }
  }

  hFrameCounter = 5;
}

// ============================================================================
// WaitForTextScrollButtonPress
// ============================================================================

extern uint8_t hDownArrowBlinkCount1;
extern uint8_t hDownArrowBlinkCount2;
extern uint8_t wTownMapSpriteBlinkingEnabled;
extern uint8_t wLinkState;

constexpr uint8_t LINK_STATE_BATTLING = 0;
constexpr uint8_t SFX_PRESS_AB = 0;

void TownMapSpriteBlinkingAnimation();
void HandleDownArrowBlinkTiming();
void CableClub_Run();
void DelayFrames(uint8_t c);
void PlaySound(uint8_t id);

inline void WaitForTextScrollButtonPress()
{
  uint8_t old1 = hDownArrowBlinkCount1;
  uint8_t old2 = hDownArrowBlinkCount2;

  hDownArrowBlinkCount1 = 0;
  hDownArrowBlinkCount2 = 6;

  while (true)
  {
    if (wTownMapSpriteBlinkingEnabled)
      TownMapSpriteBlinkingAnimation();

    HandleDownArrowBlinkTiming();
    JoypadLowSensitivity();
    CableClub_Run();

    if (hJoy5 & (A_BUTTON | B_BUTTON))
      break;
  }

  hDownArrowBlinkCount1 = old1;
  hDownArrowBlinkCount2 = old2;
}

// ============================================================================
// ManualTextScroll
// ============================================================================

inline void ManualTextScroll()
{
  if (wLinkState == LINK_STATE_BATTLING)
  {
    DelayFrames(65);
    return;
  }

  WaitForTextScrollButtonPress();
  PlaySound(SFX_PRESS_AB);
}