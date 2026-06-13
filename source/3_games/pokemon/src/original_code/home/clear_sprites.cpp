void ClearSprites(void)

{
  char cVar1;
  SpriteOAM* pSVar2;

  cVar1 = wShadowOAMEnd - wShadowOAM;
  pSVar2 = &::wShadowOAM;
  do {
    pSVar2->YCoord = 0;
    cVar1 = cVar1 + -1;
    pSVar2 = (SpriteOAM*)&pSVar2->XCoord;
  } while (cVar1 != '\0');
  return;
}

void HideSprites(void)

{
  char cVar1;
  SpriteOAM* pSVar2;

  pSVar2 = &wShadowOAM;
  cVar1 = '(';
  do {
    pSVar2->YCoord = 0xa0;
    pSVar2 = pSVar2 + 1;
    cVar1 = cVar1 + -1;
  } while (cVar1 != '\0');
  return;
}