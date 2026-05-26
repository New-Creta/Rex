// ------------------------------------------------------------
// Yes/No menu handling (translated from Game Boy assembly)
// ------------------------------------------------------------

extern void SaveScreenTilesToBuffer1();
extern void InitYesNoTextBoxParameters();
extern void DisplayTextBoxID();
extern void LoadScreenTilesFromBuffer1();

extern uint8_t wTextBoxID;
extern uint8_t wTwoOptionMenuID;

enum TextBoxID
{
  TWO_OPTION_MENU = 1
};

enum TwoOptionMenuID
{
  YES_NO_MENU = 0,
  HEAL_CANCEL_MENU = 1,
  WIDE_YES_NO_MENU = 2
};

// ------------------------------------------------------------
// YesNoChoice
// ------------------------------------------------------------
// returns: carry flag == true if YES selected
bool YesNoChoice()
{
  SaveScreenTilesToBuffer1();
  InitYesNoTextBoxParameters();
  return DisplayYesNoChoice();
}

// ------------------------------------------------------------
// TwoOptionMenu (unreferenced in original)
// ------------------------------------------------------------
void TwoOptionMenu()
{
  wTextBoxID = TWO_OPTION_MENU;
  InitYesNoTextBoxParameters();
  DisplayTextBoxID();
}

// ------------------------------------------------------------
void InitYesNoTextBoxParameters()
{
  wTwoOptionMenuID = YES_NO_MENU;

  // hlcoord 14, 7 + lb bc, 8, 15
  // (pure UI layout setup; represented abstractly)
  int x = 14;
  int y = 7;
  int width = 15;
  int height = 8;

  (void)x; (void)y; (void)width; (void)height;
}

// ------------------------------------------------------------
void YesNoChoicePokeCenter()
{
  SaveScreenTilesToBuffer1();

  wTwoOptionMenuID = HEAL_CANCEL_MENU;

  int x = 11;
  int y = 6;
  int width = 12;
  int height = 8;

  DisplayYesNoChoice();
}

// ------------------------------------------------------------
void WideYesNoChoice()
{
  SaveScreenTilesToBuffer1();

  wTwoOptionMenuID = WIDE_YES_NO_MENU;

  int x = 12;
  int y = 7;
  int width = 13;
  int height = 8;

  DisplayYesNoChoice();
}

// ------------------------------------------------------------
bool DisplayYesNoChoice()
{
  wTextBoxID = TWO_OPTION_MENU;

  DisplayTextBoxID();

  LoadScreenTilesFromBuffer1();

  // original returns via carry flag; simplified:
  // assume true = YES selected
  extern bool gYesSelected;
  return gYesSelected;
}