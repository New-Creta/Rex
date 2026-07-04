#pragma once

#include "rex_engine/engine/types.h"

namespace pokemon
{
	const s8 GBC = 0x11;

	const s8 MBC1SRamEnable = 0x0000;
	const s8 MBC1RomBank = 0x2000;
	const s8 MBC1SRamBank = 0x4000;
	const s8 MBC1SRamBankingMode = 0x6000;

	const s8 SRAM_DISABLE = 0x00;
	const s8 SRAM_ENABLE = 0x0a;

	const s8 NUM_SRAM_BANKS = 4;

	// interrupt flags
	const s8 VBLANK = 0;
	const s8 LCD_STAT = 1;
	const s8 TIMER = 2;
	const s8 SERIAL = 3;
	const s8 JOYPAD = 4;

	const s8 LY_VBLANK = 145;

	// serial
	const s8 START_TRANSFER_EXTERNAL_CLOCK = 0x80;
	const s8 START_TRANSFER_INTERNAL_CLOCK = 0x81;

	// Hardware registers
	s8* rJOYP             = (s8*)0xff00; // Joypad(R / W)
	s8* rSB               = (s8*)0xff01; // Serial transfer data(R / W)
	s8* rSC               = (s8*)0xff02; // Serial Transfer Control(R / W)
	s8* rSC_ON            = (s8*)7			; 
	s8* rSC_CGB           = (s8*)1			; 
	s8* rSC_CLOCK         = (s8*)0			; 
	s8* rDIV              = (s8*)0xff04; // Divider Register(R / W)
	s8* rTIMA             = (s8*)0xff05; // Timer counter(R / W)
	s8* rTMA              = (s8*)0xff06; // Timer Modulo(R / W)
	s8* rTAC              = (s8*)0xff07; // Timer Control(R / W)
	s8* rTAC_ON           = (s8*)2			; 
	s8* rTAC_4096_HZ      = (s8*)0			; 
	s8* rTAC_262144_HZ    = (s8*)1			; 
	s8* rTAC_65536_HZ     = (s8*)2			; 
	s8* rTAC_16384_HZ     = (s8*)3			; 
	s8* rIF               = (s8*)0xff0f; // Interrupt Flag(R / W)
	s8* rNR10             = (s8*)0xff10; // Channel 1 Sweep register (R / W)
	s8* rNR11             = (s8*)0xff11; // Channel 1 Sound length / Wave pattern duty(R / W)
	s8* rNR12             = (s8*)0xff12; // Channel 1 Volume Envelope(R / W)
	s8* rNR13             = (s8*)0xff13; // Channel 1 Fr=ency lo(Write Only)
	s8* rNR14             = (s8*)0xff14; // Channel 1 Fr=ency hi(R / W)
	s8* rNR21             = (s8*)0xff16; // Channel 2 Sound Length / Wave Pattern Duty(R / W)
	s8* rNR22             = (s8*)0xff17; // Channel 2 Volume Envelope(R / W)
	s8* rNR23             = (s8*)0xff18; // Channel 2 Fr=ency lo data(W)
	s8* rNR24             = (s8*)0xff19; // Channel 2 Fr=ency hi data(R / W)
	s8* rNR30             = (s8*)0xff1a; // Channel 3 Sound on / off(R / W)
	s8* rNR31             = (s8*)0xff1b; // Channel 3 Sound Length
	s8* rNR32             = (s8*)0xff1c; // Channel 3 Select output level(R / W)
	s8* rNR33             = (s8*)0xff1d; // Channel 3 Fr=ency's lower data (W)
	s8* rNR34             = (s8*)0xff1e; // Channel 3 Fr=ency's higher data (R/W)
	s8* rNR41             = (s8*)0xff20; // Channel 4 Sound Length(R / W)
	s8* rNR42             = (s8*)0xff21; // Channel 4 Volume Envelope(R / W)
	s8* rNR43             = (s8*)0xff22; // Channel 4 Polynomial Counter(R / W)
	s8* rNR44             = (s8*)0xff23; // Channel 4 Counter / consecutive// Initial(R / W)
	s8* rNR50             = (s8*)0xff24; // Channel control / ON - OFF / Volume(R / W)
	s8* rNR51             = (s8*)0xff25; // Selection of Sound output terminal(R / W)
	s8* rNR52             = (s8*)0xff26; // Sound on / off
	s8* rWave_0           = (s8*)0xff30; 
	s8* rWave_1           = (s8*)0xff31; 
	s8* rWave_2           = (s8*)0xff32; 
	s8* rWave_3           = (s8*)0xff33; 
	s8* rWave_4           = (s8*)0xff34; 
	s8* rWave_5           = (s8*)0xff35; 
	s8* rWave_6           = (s8*)0xff36; 
	s8* rWave_7           = (s8*)0xff37; 
	s8* rWave_8           = (s8*)0xff38; 
	s8* rWave_9           = (s8*)0xff39; 
	s8* rWave_a           = (s8*)0xff3a; 
	s8* rWave_b           = (s8*)0xff3b; 
	s8* rWave_c           = (s8*)0xff3c; 
	s8* rWave_d           = (s8*)0xff3d; 
	s8* rWave_e           = (s8*)0xff3e; 
	s8* rWave_f           = (s8*)0xff3f; 
	s8* rLCDC             = (s8*)0xff40; // LCD Control(R / W)
	s8  rLCDC_ENABLE      = 7			; 
	s8  rLCDC_ENABLE_MASK = (1 << rLCDC_ENABLE);
	s8* rSTAT             = (s8*)0xff41; // LCDC Status(R / W)
	s8* rSCY              = (s8*)0xff42; // Scroll Y(R / W)
	s8* rSCX              = (s8*)0xff43; // Scroll X(R / W)
	s8* rLY               = (s8*)0xff44; // LCDC Y - Coordinate(R)
	s8* rLYC              = (s8*)0xff45; // LY Compare(R / W)
	s8* rDMA              = (s8*)0xff46; // DMA Transfer and Start Address(W)
	s8* rBGP              = (s8*)0xff47; // BG Palette Data(R / W) - Non CGB Mode Only
	s8* rOBP0             = (s8*)0xff48; // Object Palette 0 Data(R / W) - Non CGB Mode Only
	s8* rOBP1             = (s8*)0xff49; // Object Palette 1 Data(R / W) - Non CGB Mode Only
	s8* rWY               = (s8*)0xff4a; // Window Y Position(R / W)
	s8* rWX               = (s8*)0xff4b; // Window X Position minus 7 (R / W)
	s8* rKEY1             = (s8*)0xff4d; // CGB Mode Only - Prepare Speed Switch
	s8* rVBK              = (s8*)0xff4f; // CGB Mode Only - VRAM Bank
	s8* rHDMA1            = (s8*)0xff51; // CGB Mode Only - New DMA Source, High
	s8* rHDMA2            = (s8*)0xff52; // CGB Mode Only - New DMA Source, Low
	s8* rHDMA3            = (s8*)0xff53; // CGB Mode Only - New DMA Destination, High
	s8* rHDMA4            = (s8*)0xff54; // CGB Mode Only - New DMA Destination, Low
	s8* rHDMA5            = (s8*)0xff55; // CGB Mode Only - New DMA Length / Mode / Start
	s8* rRP               = (s8*)0xff56; // CGB Mode Only - Infrared Communications Port
	s8* rBGPI             = (s8*)0xff68; // CGB Mode Only - Background Palette Index
	s8* rBGPD             = (s8*)0xff69; // CGB Mode Only - Background Palette Data
	s8* rOBPI             = (s8*)0xff6a; // CGB Mode Only - Sprite Palette Index
	s8* rOBPD             = (s8*)0xff6b; // CGB Mode Only - Sprite Palette Data
	s8* rOPRI             = (s8*)0xff6c; // CGB Mode Only - Object Priority Mode
	s8* rSVBK             = (s8*)0xff70; // CGB Mode Only - WRAM Bank
	s8* rPCM12            = (s8*)0xff76; // Channels 1 & 2 Amplitude(R)
	s8* rPCM34            = (s8*)0xff77; // Channels 3 & 4 Amplitude(R)
	s8* rIE               = (s8*)0xffff; // Interrupt Enable(R / W)
}