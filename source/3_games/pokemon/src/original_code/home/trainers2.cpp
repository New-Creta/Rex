void GetTrainerInformation()
{
  GetTrainerName();

  if (wLinkState != 0)
  {
    wTrainerPicPointer = RedPicFront;
    return;
  }

  uint8_t bank = TrainerPicAndMoneyPointers.bank;
  BankswitchHome(bank);

  uint8_t trainerClass = wTrainerClass - 1;

  uint8_t* hl = TrainerPicAndMoneyPointers.base + trainerClass * 5;

  wTrainerPicPointer = *(uint16_t*)hl;
  hl += 2;

  wTrainerBaseMoney = *(uint16_t*)hl;

  BankswitchBack();
}

// ------------------------------------------------------------

void GetTrainerName()
{
  GetTrainerName_();
}