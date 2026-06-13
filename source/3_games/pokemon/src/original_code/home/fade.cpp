void GBFadeOutToWhite(void)

{
  char extraout_B = 3;
  byte* pbVar1;

  pbVar1 = &FadePal6;
  do {
    BGP = *pbVar1;
    OBP0 = pbVar1[1];
    OBP1 = pbVar1[2];
    pbVar1 = (byte*)DelayFrames(8);
  } while (extraout_B != 0);
  return;
}