/*
 * TMC5072.h
 *
 *  Created on: 06.07.2017
 *      Author: LK
 *    Based on: TMC562-MKL.h (26.01.2012 OK)
 */

#ifndef API_IC_TMC5072_H
#define API_IC_TMC5072_H

	#include "TMC-API/tmc/helpers/API_Header.h"
	#include "TMC5072_Register.h"

	// Usage note: use 1 TypeDef per IC (LK)
	typedef struct {
		int velocity[2], oldX[2], oldTick;
		int32 registerResetState[128];
		uint8 registerAccess[128];
		bool vMaxModified[2];
	} TMC5072TypeDef;

	void tmc5072_initConfig(TMC5072TypeDef *TMC5072);
	void tmc5072_periodicJob(u8 motor, unsigned int tick, TMC5072TypeDef *TMC5072, ConfigurationTypeDef *TMC5072_config);
	u8 tmc5072_reset(ConfigurationTypeDef *TMC5072_config);
	u8 tmc5072_restore(ConfigurationTypeDef *TMC5072_config);

	/*void tmc562v3_writeDatagram(uint8 Address, uint8 x1, uint8 x2, uint8 x3, uint8 x4);
	void tmc562v3_writeInt(uint8 Address, int32 Value);
	int32 tmc562v3_readInt(uint8 Address);

	void tmc562v3_setChopperTOff(uint8 Motor, uint8 TOff);
	void tmc562v3_setChopperHysteresisStart(uint8 Motor, uint8 HysteresisStart);
	void tmc562v3_setChopperHysteresisEnd(uint8 Motor, uint8 HysteresisEnd);
	void tmc562v3_setChopperBlankTime(uint8 Motor, uint8 BlankTime);
	void tmc562v3_setChopperSync(uint8 Motor, uint8 Sync);
	void tmc562v3_setChopperMStepRes(uint8 Motor, uint8 MRes);
	void tmc562v3_setChopperDisableShortToGround(uint8 Motor, uint8 Disable);
	void tmc562v3_setChopperVHighChm(uint8 Motor, uint8 VHighChm);
	void tmc562v3_setChopperVHighFs(uint8 Motor, uint8 VHighFs);
	void tmc562v3_setChopperConstantTOffMode(uint8 Motor, uint8 ConstantTOff);
	void tmc562v3_setChopperRandomTOff(uint8 Motor, uint8 RandomTOff);
	void tmc562v3_setChopperDisableFastDecayComp(uint8 Motor, uint8 Disable);
	void tmc562v3_setChopperFastDecayTime(uint8 Motor, uint8 Time);
	void tmc562v3_setChopperSineWaveOffset(uint8 Motor, uint8 Offset);
	void tmc562v3_setChopperVSenseMode(uint8 Motor, uint8 Mode);
	uint8 tmc562v3_getChopperTOff(uint8 Motor);
	uint8 tmc562v3_getChopperHysteresisStart(uint8 Motor);
	uint8 tmc562v3_getChopperHysteresisEnd(uint8 Motor);
	uint8 tmc562v3_getChopperBlankTime(uint8 Motor);
	uint8 tmc562v3_getChopperSync(uint8 Motor);
	uint8 tmc562v3_getChopperMStepRes(uint8 Motor);
	uint8 tmc562v3_getChopperDisableShortToGround(uint8 Motor);
	uint8 tmc562v3_getChopperVHighChm(uint8 Motor);
	uint8 tmc562v3_getChopperVHighFs(uint8 Motor);
	uint8 tmc562v3_getChopperConstantTOffMode(uint8 Motor);
	uint8 tmc562v3_getChopperRandomTOff(uint8 Motor);
	uint8 tmc562v3_getChopperDisableFastDecayComp(uint8 Motor);
	uint8 tmc562v3_getChopperFastDecayTime(uint8 Motor);
	uint8 tmc562v3_getChopperSineWaveOffset(uint8 Motor);
	uint8 tmc562v3_getChopperVSenseMode(uint8 Motor);
	void tmc562v3_setSmartEnergyUpStep(uint8 Motor, uint8 UpStep);
	void tmc562v3_setSmartEnergyDownStep(uint8 Motor, uint8 DownStep);
	void tmc562v3_setSmartEnergyStallLevelMax(uint8 Motor, uint8 Max);
	void tmc562v3_setSmartEnergyStallLevelMin(uint8 Motor, uint8 Min);
	void tmc562v3_setSmartEnergyStallThreshold(uint8 Motor, int8 Threshold);
	void tmc562v3_setSmartEnergyIMin(uint8 Motor, uint8 IMin);
	void tmc562v3_setSmartEnergyFilter(uint8 Motor, uint8 Filter);
	uint8 tmc562v3_getSmartEnergyUpStep(uint8 Motor);
	uint8 tmc562v3_getSmartEnergyDownStep(uint8 Motor);
	uint8 tmc562v3_getSmartEnergyStallLevelMax(uint8 Motor);
	uint8 tmc562v3_getSmartEnergyStallLevelMin(uint8 Motor);
	int32 tmc562v3_getSmartEnergyStallThreshold(uint8 Motor);
	uint8 tmc562v3_getSmartEnergyIMin(uint8 Motor);
	uint8 tmc562v3_getSmartEnergyFilter(uint8 Motor);
	void tmc562v3_setPwmAmpl(uint8 Motor, uint8 PwmAmpl);
	void tmc562v3_setPwmGrad(uint8 Motor, uint8 PwmGrad);
	void tmc562v3_setPwmFreq(uint8 Motor, uint8 PwmFreq);
	void tmc562v3_setPwmAutoscale(uint8 Motor, uint8 Autoscale);
	void tmc562v3_setFreewheel(uint8 Motor, uint8 Freewheel);
	uint8 tmc562v3_getPwmAmpl(uint8 Motor);
	uint8 tmc562v3_getPwmGrad(uint8 Motor);
	uint8 tmc562v3_getPwmFreq(uint8 Motor);
	uint8 tmc562v3_getPwmAutoscale(uint8 Motor);
	uint8 tmc562v3_getPwmFreewheel(uint8 Motor);
	uint8 tmc562v3_getPwmStatus(uint8 Motor);

	void tmc562v3_initMotorDrivers(void);
	void tmc562v3_hardStop(uint8 Motor);*/

#endif
