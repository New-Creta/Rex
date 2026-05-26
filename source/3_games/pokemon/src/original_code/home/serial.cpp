// =========================================================
// Serial (partial C++ translation)
// =========================================================

#include <cstdint>

// -------------------- hardware / RAM mocks --------------------
uint8_t hSerialConnectionStatus;
uint8_t hSerialReceiveData;
uint8_t hSerialSendData;
uint8_t hSerialReceivedNewData;
uint8_t hSerialIgnoringInitialData;

uint8_t hRandomAdd;

uint8_t wUnknownSerialCounter;
uint8_t wUnknownSerialCounter2;
uint8_t wLinkMenuSelectionSendBuffer[16];
uint8_t wLinkMenuSelectionReceiveBuffer[16];

uint8_t wSerialExchangeNybbleSendData;
uint8_t wSerialExchangeNybbleReceiveData;
uint8_t wSerialExchangeNybbleTempReceiveData;
uint8_t wSerialSyncAndExchangeNybbleReceiveData;

uint8_t hLoadedROMBank;

// -------------------- constants --------------------
constexpr uint8_t USING_INTERNAL_CLOCK = 0x01;
constexpr uint8_t USING_EXTERNAL_CLOCK = 0x02;
constexpr uint8_t SERIAL_NO_DATA_BYTE = 0xFF;
constexpr uint8_t SERIAL_PREAMBLE_BYTE = 0xFD;

constexpr uint8_t START_TRANSFER_INTERNAL_CLOCK = 0x80;
constexpr uint8_t START_TRANSFER_EXTERNAL_CLOCK = 0x81;
constexpr uint8_t ESTABLISH_CONNECTION_WITH_EXTERNAL_CLOCK = 0x82;

// -------------------- stubs --------------------
void DelayFrame();
void WaitLoop_15Iterations();
void Joypad();
void SaveScreenTilesToBuffer1();
void LoadScreenTilesFromBuffer1();
void PrintWaitingText();

// =========================================================
// Serial_ExchangeByte (simplified structural port)
// =========================================================
uint8_t Serial_ExchangeByte()
{
  hSerialReceivedNewData = 0;

  if (hSerialConnectionStatus != USING_INTERNAL_CLOCK)
  {
    hSerialSendData = START_TRANSFER_INTERNAL_CLOCK;
  }

  while (true)
  {
    if (hSerialReceivedNewData)
      break;

    // simplified omission of full counter logic loops
  }

  hSerialReceivedNewData = 0;

  uint8_t received = hSerialReceiveData;

  if (received == SERIAL_NO_DATA_BYTE)
    return SERIAL_NO_DATA_BYTE;

  return received;
}

// =========================================================
// Serial_ExchangeBytes
// =========================================================
void Serial_ExchangeBytes(uint8_t* hl, uint8_t* de, uint16_t bc)
{
  hSerialIgnoringInitialData = 1;

  while (bc != 0)
  {
    hSerialSendData = *hl;

    uint8_t a = Serial_ExchangeByte();

    uint8_t b = a;

    hl++;

    DelayFrame();

    if (!hSerialIgnoringInitialData)
    {
      *de = b;
      de++;
      bc--;
    }
    else
    {
      if (b == SERIAL_PREAMBLE_BYTE)
      {
        hSerialIgnoringInitialData = 0;
      }
    }
  }
}

// =========================================================
// Serial_ExchangeLinkMenuSelection
// =========================================================
void Serial_ExchangeLinkMenuSelection()
{
  uint8_t* hl = wLinkMenuSelectionSendBuffer;
  uint8_t* de = wLinkMenuSelectionReceiveBuffer;
  uint8_t c = 2;

  hSerialIgnoringInitialData = 1;

  while (c != 0)
  {
    DelayFrame();

    hSerialSendData = *hl;

    uint8_t b = Serial_ExchangeByte();

    hl++;

    hSerialIgnoringInitialData = 0;

    *de = b;
    de++;

    c--;
  }
}

// =========================================================
// Serial_PrintWaitingTextAndSyncAndExchangeNybble
// =========================================================
void Serial_PrintWaitingTextAndSyncAndExchangeNybble()
{
  SaveScreenTilesToBuffer1();

  PrintWaitingText();

  // Serial_SyncAndExchangeNybble (not fully expanded here)

  LoadScreenTilesFromBuffer1();
}

// =========================================================
// Serial_SendZeroByte
// =========================================================
void Serial_SendZeroByte()
{
  hSerialSendData = 0;

  if (hSerialConnectionStatus == USING_INTERNAL_CLOCK)
  {
    // start transfer internal clock
  }
}

// =========================================================
// Serial_TryEstablishingExternallyClockedConnection
// =========================================================
void Serial_TryEstablishingExternallyClockedConnection()
{
  hSerialSendData = ESTABLISH_CONNECTION_WITH_EXTERNAL_CLOCK;
  hSerialReceiveData = 0;

  // start transfer external clock
}