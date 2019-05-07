/*
 * TMC5072.c
 *
 *  Created on: 06.07.2017
 *      Author: LK
 *    Based on: TMC562-MKL.h (26.01.2012 OK)
 */

#include "TMC5072.h"

#define R30 0x00<<24 	| 0x07<<16	| 0x17<<8	| 0x03
#define R32 0xFFFFFF
#define R3A 65536
#define R60 0b10101010101010101011010101010100
#define R61 0b01001010100101010101010010101010
#define R62 0b00100100010010010010100100101001
#define R63 0b00010000000100000100001000100010
#define R64 0b11111011111111111111111111111111
#define R65 0b10110101101110110111011101111101
#define R66 0b01001001001010010101010101010110
#define R67 0b00000000010000000100001000100010
#define R68 255<<24 	| 255<<16 	| 0x80<<8 	| 0b01010110
#define R69 247<<16
#define R6C 0x00<<24 	| 0x01<<16	| 0x01<<8	| 0xd5

const u8 tmc5072_defaultRegisterAccess[128] = {
		3, 1, 1, 2, 7, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		2, 1, 0, 0, 0, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0,
		3, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0,
		2, 2, 2, 2, 3, 1, 1, 0, 3, 3, 2, 1, 1, 0, 0, 0,
		3, 3, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 3, 0, 0,
		2, 2, 2, 2, 3, 1, 1, 0, 3, 3, 2, 1, 1, 0, 0, 0,
		2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 1, 3, 2, 2, 1,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 2, 2, 1 };
const s32 tmc5072_defaultRegisterResetState[128] = {
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		R30, 0, R32, 0, 0, 0, 0, 0, 0, 0, R3A, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
		R30, 0, R32, 0, 0, 0, 0, 0, 0, 0, R3A, 0, 0, 0, 0, 0,
		R60, R61, R62, R63, R64, R65, R66, R67, R68, R69, 0, 0, R6C, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, R6C, 0, 0, 0 };

// => SPI wrapper
extern void tmc5072_writeDatagram(uint8 address, uint8 x1, uint8 x2, uint8 x3, uint8 x4);
extern void tmc5072_writeInt(uint8 address, int value);
extern int tmc5072_readInt(uint8 address);
// <= SPI wrapper

void tmc5072_initConfig(TMC5072TypeDef *tmc5072)
{
	tmc5072->velocity[0] = 0;
	tmc5072->velocity[1] = 0;
	tmc5072->oldTick = 0;
	tmc5072->oldX[0] = 0;
	tmc5072->oldX[1] = 0;
	tmc5072->vMaxModified[0] = FALSE;
	tmc5072->vMaxModified[1] = FALSE;

	int i;
	for (i=0; i < 128; i++)
	{
		tmc5072->registerAccess[i] = tmc5072_defaultRegisterAccess[i];
		tmc5072->registerResetState[i] = tmc5072_defaultRegisterResetState[i];
	}
};

void tmc5072_writeConfiguration(TMC5072TypeDef *tmc5072, ConfigurationTypeDef *TMC5072_config)
{
	uint8 *ptr				= &TMC5072_config->ptr;
	const int32 *settings	= (TMC5072_config->isBusy == 2) ? TMC5072_config->shadowRegister : tmc5072->registerResetState;

	while(!(tmc5072->registerAccess[*ptr]&2) && (*ptr<128)) (*ptr)++;
	if(*ptr<128)
	{
		tmc5072_writeInt(*ptr, settings[*ptr]);
		(*ptr)++;
	}
	else
	{
		TMC5072_config->isBusy = 0;
	}
}

void tmc5072_periodicJob(u8 motor, unsigned int tick, TMC5072TypeDef *tmc5072, ConfigurationTypeDef *TMC5072_config)
{
	int xActual, t;

	if(TMC5072_config->isBusy)
	{
		tmc5072_writeConfiguration(tmc5072, TMC5072_config);
		return;
	}

	if((t = abs(tick-tmc5072->oldTick)) >= 5)
	{

		xActual	= tmc5072_readInt(TMC5072_XACTUAL|MOTOR_ADDR(motor));
		TMC5072_config->shadowRegister[TMC5072_XACTUAL|MOTOR_ADDR(motor)] = xActual;
		tmc5072->velocity[motor] = (int) ((float) (abs(xActual-tmc5072->oldX[motor]) / (float) t) 	* (float) 1048.576);
		tmc5072->oldX[motor]	= xActual;

		// Not per motor:
		/*xActual	= tmc5072_readInt(motor, TMC5072_XACTUAL_1);
		TMC562v3_config->shadowRegister[TMC5072_XACTUAL_1] = xActual;
		TMC562V3.velocityMotor1 = (int) ((float) (abs(xActual-oldX[0]) / (float) t) 	* (float) 1048.576);
		tmc5072->oldX	= xActual;

		xActual	= readInt(TMC5072_XACTUAL_2);
		TMC562v3_config->shadowRegister[TMC5072_XACTUAL_2] = xActual;
		TMC562V3.velocityMotor2 = (int) ((float) (abs(xActual-oldX[1]) / (float) t)	* (float) 1048.576);
		tmc5072->oldX	= xActual;*/

		tmc5072->oldTick=tick;
	}
}

uint8 tmc5072_reset(ConfigurationTypeDef *TMC5072_config)
{
	if(TMC5072_config->isBusy) return 0;
	TMC5072_config->isBusy 			= 1;
	TMC5072_config->ptr	 			= 0;
	return 1;
}

uint8 tmc5072_restore(ConfigurationTypeDef *TMC5072_config)
{
	if(TMC5072_config->isBusy) return 0;
	TMC5072_config->isBusy 			= 2;
	TMC5072_config->ptr	 			= 0;
	return 1;
}

//#define VEL_FACTOR 0.953674316406         //fClk/2 / 2^23   (fClk=16MHz)
//#define ACC_FACTOR 116.415321827          //fClk^2 / (512*256) / 2^24   (fClk=8MHz)
//
////Table that shows whether each TMC562 register can be read (1) or not(0)
//static const uint8 TMC562v3RegisterReadable[128]={
//1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    //00..0f
//0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0,    //10..1f
//1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //20..2f
//0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //30..3f
//1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0,    //40..4f
//0, 0, 0, 0, 1, 1, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0,    //50..5f
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //60..6f
//0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 1,    //70..7f
//};
//
//static int32 TMC562v3SoftwareCopy[128];	//Software copy for all TMC562 registers (not only readable ones)
//
//
///*******************************************************************
//   Function: tmc5072_writeDatagram()
//   Parameter: Address: Register address (0x00..0x7f)
//              x1, x2, x3, x4: Each byte of the register
//
//   Returns: ---
//
//   Purpose: Write a TMC562 register, giving the value of each byte.
//   	   	  The software copy of the register will be as well updated.
//********************************************************************/
//void tmc5072_writeDatagram(uint8 Address, uint8 x1, uint8 x2, uint8 x3, uint8 x4)
//{
//  int32 Value;
//
//  Value=x1;
//  Value<<=8;
//  Value|=x2;
//  Value<<=8;
//  Value|=x3;
//  Value<<=8;
//  Value|=x4;
//
//  //Write register in TMC562
//  tmc5072_spi_writeInt(Address, Value);
//
//  //Update software copy
//  TMC562v3SoftwareCopy[Address & 0x7f]=Value;
//}
//
//
///*******************************************************************
//   Function: tmc5072_writeInt()
//   Parameter: Address: Register address (0x00..0x7f)
//              Value: value to be written
//
//   Returns: ---
//
//   Purpose: Write a TMC562 register. The software copy of the
//   	   	  register will be as well updated.
//********************************************************************/
//void tmc5072_writeInt(uint8 Address, int32 Value)
//{
//  //Write register in TMC562
//  tmc5072_spi_writeInt(Address, Value);
//
//  //Update software copy
//  TMC562v3SoftwareCopy[Address & 0x7f]=Value;
//}
//
//
///*******************************************************************
//   Function: tmc5072_readInt()
//   Parameter: Address: Register address (0x00..0x7f)
//
//   Returns: read value
//
//   Purpose: Read the value of a TMC562 register. If not readable,
//   	   	  the value from the software copy will be returned.
//********************************************************************/
//int32 tmc5072_readInt(uint8 Address)
//{
//  int32 Value;
//
//  Address&=0x7f;
//  if(TMC562v3RegisterReadable[Address])
//  {
//    //Register is readable -> read from TMC562
//    //Two accesses are required
//    tmc5072_spi_readInt(Address);
//
//    //For the second read access, address 0 (GCONF) will be used, so that
//    //e.g. a non-read event bit won't be erased by mistake.
//    Value = tmc5072_spi_readInt(0);
//
//    //Register VACTUAL has only 24 bits => set sign-bit
//    if(Address==0x22 || Address==0x42)
//    {
//      if(Value & BIT23) Value|=0xff000000;
//    }
//
//    return Value;
//  }
//  else
//  {
//    //Register not readable => return software copy
//    return TMC562v3SoftwareCopy[Address];
//  }
//}
//
//
///*******************************************************************
//  Set and get functions for each setting of the CHOPCONF register
//********************************************************************/
//void tmc5072_setChopperTOff(uint8 Motor, uint8 TOff)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffffff0;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | (TOff & 0x0f));
//}
//
//void tmc5072_setChopperHysteresisStart(uint8 Motor, uint8 HysteresisStart)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xffffff8f;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((HysteresisStart & 0x07) << 4));
//}
//
//void tmc5072_setChopperHysteresisEnd(uint8 Motor, uint8 HysteresisEnd)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffff87f;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((HysteresisEnd & 0x0f) << 7));
//}
//
//void tmc5072_setChopperBlankTime(uint8 Motor, uint8 BlankTime)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffe7fff;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((BlankTime & 0x03) << 15));
//}
//
//void tmc5072_setChopperSync(uint8 Motor, uint8 Sync)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xff0fffff;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((Sync & 0x0f) << 20));
//}
//
//void tmc5072_setChopperMStepRes(uint8 Motor, uint8 MRes)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xf0ffffff;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((MRes & 0x0f) << 24));
//}
//
//void tmc5072_setChopperDisableShortToGround(uint8 Motor, uint8 Disable)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(Disable)
//    Value|=BIT30;
//  else
//    Value&= ~BIT30;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperVHighChm(uint8 Motor, uint8 VHighChm)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(VHighChm)
//    Value|=BIT19;
//  else
//    Value&= ~BIT19;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperVHighFs(uint8 Motor, uint8 VHighFs)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(VHighFs)
//    Value|=BIT18;
//  else
//    Value&= ~BIT18;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperConstantTOffMode(uint8 Motor, uint8 ConstantTOff)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(ConstantTOff)
//    Value|=BIT14;
//  else
//    Value&= ~BIT14;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperRandomTOff(uint8 Motor, uint8 RandomTOff)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(RandomTOff)
//    Value|=BIT13;
//  else
//    Value&= ~BIT13;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperDisableFastDecayComp(uint8 Motor, uint8 Disable)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  if(Disable)
//    Value|=BIT12;
//  else
//    Value&= ~BIT12;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setChopperFastDecayTime(uint8 Motor, uint8 Time)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xffffff8f;
//
//  if(Time & BIT3)
//    Value|=BIT11;
//  else
//    Value&= ~BIT11;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((Time & 0x07) << 4));
//}
//
//void tmc5072_setChopperSineWaveOffset(uint8 Motor, uint8 Offset)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffff87f;
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value | ((Offset & 0x0f) << 7));
//}
//
//void tmc5072_setChopperVSenseMode(uint8 Motor, uint8 Mode)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//
//  if(Mode)
//    Value|=BIT17;
//  else
//    Value&= ~BIT17;
//
//  tmc5072_writeInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//uint8 tmc5072_getChopperTOff(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & 0x0000000f;
//}
//
//uint8 tmc5072_getChopperHysteresisStart(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 4) & 0x07;
//}
//
//uint8 tmc5072_getChopperHysteresisEnd(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 7) & 0x0f;
//}
//
//uint8 tmc5072_getChopperBlankTime(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 15) & 0x03;
//}
//
//uint8 tmc5072_getChopperSync(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 20) & 0x0f;
//}
//
//uint8 tmc5072_getChopperMStepRes(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 24) & 0x0f;
//}
//
//uint8 tmc5072_getChopperDisableShortToGround(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT30 ? 1:0;
//}
//
//uint8 tmc5072_getChopperVHighChm(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT19 ? 1:0;
//}
//
//uint8 tmc5072_getChopperVHighFs(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT18 ? 1:0;
//}
//
//uint8 tmc5072_getChopperConstantTOffMode(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT14 ? 1:0;
//}
//
//uint8 tmc5072_getChopperRandomTOff(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT13 ? 1:0;
//}
//
//uint8 tmc5072_getChopperDisableFastDecayComp(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT12 ? 1:0;
//}
//
//uint8 tmc5072_getChopperFastDecayTime(uint8 Motor)
//{
//  uint32 Value;
//  uint8 Time;
//
//  Value=tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor));
//  Time=(Value >> 4) & 0x07;
//  if(Value & BIT11) Time|=BIT3;
//
//  return Time;
//}
//
//uint8 tmc5072_getChopperSineWaveOffset(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) >> 7) & 0x0f;
//}
//
//uint8 tmc5072_getChopperVSenseMode(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_CHOPCONF|MOTOR_ADDR_DRV(Motor)) & BIT17 ? 1:0;
//}
//
//
///*******************************************************************
//  Set and get functions for each setting of the COOLCONF register
//********************************************************************/
//void tmc5072_setSmartEnergyUpStep(uint8 Motor, uint8 UpStep)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0xffffff9f;
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value | ((UpStep & 0x03) << 5));
//}
//
//void tmc5072_setSmartEnergyDownStep(uint8 Motor, uint8 DownStep)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0xffff9fff;
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value | ((DownStep & 0x03) << 13));
//}
//
//void tmc5072_setSmartEnergyStallLevelMax(uint8 Motor, uint8 Max)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffff0ff;
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value | ((Max & 0x0f) << 8));
//}
//
//void tmc5072_setSmartEnergyStallLevelMin(uint8 Motor, uint8 Min)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0xfffffff0;
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value | (Min & 0x0f));
//}
//
//void tmc5072_setSmartEnergyStallThreshold(uint8 Motor, int8 Threshold)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0xff00ffff;
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value | ((Threshold & 0xff) << 16));
//}
//
//void tmc5072_setSmartEnergyIMin(uint8 Motor, uint8 IMin)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor));
//  if(IMin)
//    Value|=BIT15;
//  else
//    Value&= ~BIT15;
//
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//void tmc5072_setSmartEnergyFilter(uint8 Motor, uint8 Filter)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor));
//  if(Filter)
//    Value|=BIT24;
//  else
//    Value&= ~BIT24;
//
//  tmc5072_writeInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor), Value);
//}
//
//
//uint8 tmc5072_getSmartEnergyUpStep(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) >> 5) & 0x03;
//}
//
//uint8 tmc5072_getSmartEnergyDownStep(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) >> 13) & 0x03;
//}
//
//uint8 tmc5072_getSmartEnergyStallLevelMax(uint8 Motor)
//{
//  return (tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) >> 8) & 0x0f;
//}
//
//uint8 tmc5072_getSmartEnergyStallLevelMin(uint8 Motor)
//{
//  return tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & 0x0f;
//}
//
//int32 tmc5072_getSmartEnergyStallThreshold(uint8 Motor)
//{
//  int32 Value;
//
//  Value=(tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) >> 16) & 0xff;
//  if(Value & BIT7) Value|=0xffffff00;
//
//  return Value;
//}
//
//uint8 tmc5072_getSmartEnergyIMin(uint8 Motor)
//{
//  if(tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & BIT15)
//    return 1;
//  else
//    return 0;
//}
//
//uint8 tmc5072_getSmartEnergyFilter(uint8 Motor)
//{
//  if(tmc5072_readInt(TMC5072_COOLCONF|MOTOR_ADDR_DRV(Motor)) & BIT24)
//    return 1;
//  else
//    return 0;
//}
//
///*******************************************************************
//  Set and get functions for each setting of the PWMCONF register
//********************************************************************/
//void tmc5072_setPwmAmpl(uint8 Motor, uint8 PwmAmpl)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1);
//  tmc5072_writeInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1,
//    (Value & 0xffffff00) | PwmAmpl);
//}
//
//void tmc5072_setPwmGrad(uint8 Motor, uint8 PwmGrad)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1);
//  tmc5072_writeInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1,
//    (Value & 0xffff00ff) | (PwmGrad << 8));
//}
//
//void tmc5072_setPwmFreq(uint8 Motor, uint8 PwmFreq)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1);
//  tmc5072_writeInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1,
//    (Value & 0xfffcffff) | ((PwmFreq &0x03) << 16));
//}
//
//void tmc5072_setPwmAutoscale(uint8 Motor, uint8 Autoscale)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) & 0xfffbffff;
//  if(Autoscale) Value|=BIT18;
//  tmc5072_writeInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1, Value);
//}
//
//void tmc5072_setPwmFreewheel(uint8 Motor, uint8 Freewheel)
//{
//  uint32 Value;
//
//  Value=tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1);
//  tmc5072_writeInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1,
//    (Value & 0xff3fffff) | ((Freewheel &0x03) << 20));
//}
//
//
//uint8 tmc5072_getPwmAmpl(uint8 Motor)
//{
//  return tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) & 0xff;
//}
//
//uint8 tmc5072_getPwmGrad(uint8 Motor)
//{
//  return (tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) >> 8) & 0xff;
//}
//
//uint8 tmc5072_getPwmFreq(uint8 Motor)
//{
//  return (tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) >> 16) & 0x03;
//}
//
//uint8 tmc5072_getPwmAutoscale(uint8 Motor)
//{
//  return (tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) >> 18) & 0x01;
//}
//
//uint8 tmc5072_getPwmFreewheel(uint8 Motor)
//{
//  return (tmc5072_readInt((Motor==1) ? TMC5072_PWMCONF_2 : TMC5072_PWMCONF_1) >> 20) & 0x03;
//}
//
//uint8 tmc5072_getPwmStatus(uint8 Motor)
//{
//  return tmc5072_readInt((Motor==1) ? TMC5072_PWM_STATUS_2 : TMC5072_PWM_STATUS_1) & 0xff;
//}
//
//
///*******************************************************************
//   Function: tmc5072_initMotorDrivers()
//   Parameter: ---
//
//   Returns: ---
//
//   Purpose: initialize TMC562.
//********************************************************************/
//void tmc5072_initMotorDrivers(void)
//{
//  //2-phase configuration Motor 1
//  tmc5072_writeDatagram(TMC5072_CHOPCONF_1, 0x00, 0x01, 0x01, 0x35);
//  tmc5072_writeDatagram(TMC5072_IHOLD_IRUN_1, 0x00, 0x07, 0x14, 0x00);
//
//  //2-phase configuration Motor 2
//  tmc5072_writeDatagram(TMC5072_CHOPCONF_2, 0x00, 0x01, 0x01, 0x35);
//  tmc5072_writeDatagram(TMC5072_IHOLD_IRUN_2, 0x00, 0x07, 0x14, 0x00);
//
//  //no parallel turn off/on of the driver
//  tmc5072_writeInt(0x00, tmc5072_readInt(0x00) & ~BIT0);
//
//  //turn on PP and INT outputs (so that they don't float)
//  tmc5072_writeInt(0x00, tmc5072_readInt(0x00) | BIT3);
//
//  //Reset positions
//  tmc5072_writeInt(TMC5072_RAMPMODE_1, TMC5072_MODE_POSITION);
//  tmc5072_writeInt(TMC5072_XTARGET_1, 0);
//  tmc5072_writeInt(TMC5072_XACTUAL_1, 0);
//  tmc5072_writeInt(TMC5072_RAMPMODE_2, TMC5072_MODE_POSITION);
//  tmc5072_writeInt(TMC5072_XTARGET_2, 0);
//  tmc5072_writeInt(TMC5072_XACTUAL_2, 0);
//
//  //Standard values for speed and acceleration
//  tmc5072_writeInt(TMC5072_VSTART_1, 1);
//  tmc5072_writeInt(TMC5072_A1_1, 220);
//  tmc5072_writeInt(TMC5072_V1_1, 26843);
//  tmc5072_writeInt(TMC5072_AMAX_1, 439);    //51200pps/s (for 16MHz)
//  tmc5072_writeInt(TMC5072_VMAX_1, 53687);  //51200pps   (for 16MHz)
//  tmc5072_writeInt(TMC5072_DMAX_1, 439);
//  tmc5072_writeInt(TMC5072_D1_1, 220);
//  tmc5072_writeInt(TMC5072_VSTOP_1, 10);
//
//  tmc5072_writeInt(TMC5072_VSTART_2, 1);
//  tmc5072_writeInt(TMC5072_A1_2, 220);
//  tmc5072_writeInt(TMC5072_V1_2, 26843);
//  tmc5072_writeInt(TMC5072_AMAX_2, 439);
//  tmc5072_writeInt(TMC5072_VMAX_2, 53687);
//  tmc5072_writeInt(TMC5072_DMAX_2, 439);
//  tmc5072_writeInt(TMC5072_D1_2, 220);
//  tmc5072_writeInt(TMC5072_VSTOP_2, 10);
//}
//
//
///*******************************************************************
//   Function: tmc5072_hardStop()
//   Parameter: Motor: motor number (0..N_O_MOTORS)
//   Returns: ---
//
//   Purpose: Stop a motor immediately
//********************************************************************/
//void tmc5072_hardStop(uint8 Motor)
//{
//  tmc5072_writeInt(TMC5072_VMAX|MOTOR_ADDR(Motor), 0);
//  tmc5072_writeDatagram(TMC5072_RAMPMODE|MOTOR_ADDR(Motor), 0, 0, 0, TMC5072_MODE_VELPOS);
//}
