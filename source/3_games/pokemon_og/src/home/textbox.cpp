void DisplayTextBoxID(uint8_t textBoxID, uint8_t y, uint8_t x)
{
  // [wTextBoxID] = text box ID
  wTextBoxID = textBoxID;

  // b, c = y, x cursor position (TWO_OPTION_MENU only)
  wCursorY = y;
  wCursorX = x;

  DisplayTextBoxID_();
}