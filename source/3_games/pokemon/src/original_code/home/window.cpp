#include <cstdint>

// ------------------------------------------------------------
// Assumed external game state / hardware abstraction
// ------------------------------------------------------------
extern uint8_t wPartyMenuAnimMonEnabled;
extern uint8_t wAnimCounter;
extern uint8_t wMenuJoypadPollCount;
extern uint8_t wMenuWrappingEnabled;
extern uint8_t wCurrentMenuItem;
extern uint8_t wMaxMenuItem;
extern uint8_t wMenuWatchedKeys;
extern uint8_t wMenuWatchMovingOutOfBounds;
extern uint8_t wCheckFor180DegreeTurn;
extern uint8_t wTopMenuItemY;
extern uint8_t wTopMenuItemX;
extern uint8_t wLastMenuItem;
extern uint8_t wTileBehindCursor;
extern uint16_t wMenuCursorLocation;
extern uint8_t wFlags_0xcd60;
extern uint8_t wAutoTextBoxDrawingControl;
extern uint8_t wDoNotWaitForButtonPressAfterDisplayingText;
extern uint8_t wTextBoxID;

extern uint8_t hDownArrowBlinkCount1;
extern uint8_t hDownArrowBlinkCount2;
extern uint8_t hJoy5;
extern uint8_t hUILayoutFlags;

extern uint8_t SFX_PRESS_AB;
extern uint8_t A_BUTTON;
extern uint8_t B_BUTTON;
extern uint8_t BIT_D_UP;
extern uint8_t BIT_D_DOWN;

extern void AnimatePartyMon();
extern void JoypadLowSensitivity();
extern void HandleDownArrowBlinkTiming(uint8_t* hl);
extern void PlaySound(uint8_t);
extern void DisplayTextBoxID();
extern void UpdateSprites();
extern void Delay3();
extern void TextCommandProcessor();
extern void farcall_AnimatePartyMon();

// ------------------------------------------------------------
// HandleMenuInput
// ------------------------------------------------------------
void HandleMenuInput()
{
  wPartyMenuAnimMonEnabled = 0;

  uint8_t saved1 = hDownArrowBlinkCount1;
  uint8_t saved2 = hDownArrowBlinkCount2;

  hDownArrowBlinkCount1 = 0;
  hDownArrowBlinkCount2 = 6;

  while (true)
  {
    wAnimCounter = 0;

    PlaceMenuCursor();
    Delay3();

    if (wPartyMenuAnimMonEnabled)
      farcall_AnimatePartyMon();

    JoypadLowSensitivity();

    uint8_t joy = hJoy5;

    if (joy != 0)
    {
      wCheckFor180DegreeTurn = 0;

      // UP
      if (joy & BIT_D_UP)
      {
        if (wCurrentMenuItem == 0)
        {
          if (!wMenuWrappingEnabled)
            goto input_done;

          wCurrentMenuItem = wMaxMenuItem;
        }
        else
        {
          wCurrentMenuItem--;
        }
      }

      // DOWN
      else if (joy & BIT_D_DOWN)
      {
        uint8_t next = wCurrentMenuItem + 1;

        if (next > wMaxMenuItem)
        {
          if (!wMenuWrappingEnabled)
            goto input_done;

          wCurrentMenuItem = 0;
        }
        else
        {
          wCurrentMenuItem = next;
        }
      }

      if ((wMenuWatchedKeys & joy) == 0)
        continue;

      if (joy & (A_BUTTON | B_BUTTON))
      {
        if (!(wFlags_0xcd60 & (1 << 5)))
          PlaySound(SFX_PRESS_AB);
      }

      hDownArrowBlinkCount1 = saved1;
      hDownArrowBlinkCount2 = saved2;
      wMenuWrappingEnabled = 0;

      return;
    }

    // blinking arrow handling (simplified)
    HandleDownArrowBlinkTiming(nullptr);

    if (--wMenuJoypadPollCount == 0)
      break;
  }

input_done:
  hDownArrowBlinkCount1 = saved1;
  hDownArrowBlinkCount2 = saved2;
  wMenuWrappingEnabled = 0;
}

// ------------------------------------------------------------
// PlaceMenuCursor
// ------------------------------------------------------------
void PlaceMenuCursor()
{
  // simplified placeholder: exact VRAM coordinate math omitted
  uint8_t item = wCurrentMenuItem;

  if (wLastMenuItem != 0)
  {
    // erase old cursor
    // (placeholder)
  }

  // place new cursor
  wTileBehindCursor = 0;
  wMenuCursorLocation = 0;
  wLastMenuItem = item;
}

// ------------------------------------------------------------
void PlaceUnfilledArrowMenuCursor(uint8_t a)
{
  uint16_t loc = wMenuCursorLocation;
  *((uint8_t*)loc) = '▷';
}

// ------------------------------------------------------------
void EraseMenuCursor()
{
  uint16_t loc = wMenuCursorLocation;
  *((uint8_t*)loc) = ' ';
}

// ------------------------------------------------------------
void HandleDownArrowBlinkTiming(uint8_t* hl)
{
  // simplified behavior
  if (!hl) return;
}

// ------------------------------------------------------------
void EnableAutoTextBoxDrawing()
{
  wAutoTextBoxDrawingControl = 0;
  wDoNotWaitForButtonPressAfterDisplayingText = 0;
}

void DisableAutoTextBoxDrawing()
{
  wAutoTextBoxDrawingControl = 1;
  wDoNotWaitForButtonPressAfterDisplayingText = 0;
}

// ------------------------------------------------------------
void PrintText(uint8_t* hl)
{
  wTextBoxID = 0; // MESSAGE_BOX

  DisplayTextBoxID();
  UpdateSprites();
  Delay3();

  TextCommandProcessor();
}