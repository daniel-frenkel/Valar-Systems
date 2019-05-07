/* 
 * TMC5072-EVAL_UART.ino
 * 
 * Created on: 20.02.2017
 *     Author: MN
 */
 
#ifndef TMC5072_REGISTER_H
#define TMC5072_REGISTER_H
 
	// ===== TMC5072 register set =====
 
	#define TMC5072_GCONF        0x00
	#define TMC5072_GSTAT        0x01
	#define TMC5072_IFCNT        0x02
	#define TMC5072_SLAVECONF    0x03
	#define TMC5072_INP_OUT      0x04
	#define TMC5072_X_COMPARE    0x05
 
	#define TMC5072_PWMCONF_1	  0x10
	#define TMC5072_PWM_STATUS_1 0x11
 
	#define TMC5072_PWMCONF_2	  0x18
	#define TMC5072_PWM_STATUS_2 0x19
 
	#define TMC5072_RAMPMODE_1   0x20
	#define TMC5072_XACTUAL_1    0x21
	#define TMC5072_VACTUAL_1    0x22
	#define TMC5072_VSTART_1     0x23
	#define TMC5072_A1_1         0x24
	#define TMC5072_V1_1         0x25
	#define TMC5072_AMAX_1       0x26
	#define TMC5072_VMAX_1       0x27
	#define TMC5072_DMAX_1       0x28
	#define TMC5072_D1_1         0x2A
	#define TMC5072_VSTOP_1      0x2B
	#define TMC5072_TZEROWAIT_1  0x2C
	#define TMC5072_XTARGET_1    0x2D
	#define TMC5072_IHOLD_IRUN_1 0x30
	#define TMC5072_VCOOLTHRS_1  0x31
	#define TMC5072_VHIGH_1      0x32
	#define TMC5072_VDCMIN_1     0x33
	#define TMC5072_SWMODE_1     0x34
	#define TMC5072_RAMPSTAT_1   0x35
	#define TMC5072_XLATCH_1     0x36
	#define TMC5072_ENCMODE_1    0x38
	#define TMC5072_XENC_1       0x39
	#define TMC5072_ENC_CONST_1  0x3A
	#define TMC5072_ENC_STATUS_1 0x3B
	#define TMC5072_ENC_LATCH_1  0x3C
 
	#define TMC5072_RAMPMODE_2   0x40
	#define TMC5072_XACTUAL_2    0x41
	#define TMC5072_VACTUAL_2    0x42
	#define TMC5072_VSTART_2     0x43
	#define TMC5072_A1_2         0x44
	#define TMC5072_V1_2         0x45
	#define TMC5072_AMAX_2       0x46
	#define TMC5072_VMAX_2       0x47
	#define TMC5072_DMAX_2       0x48
	#define TMC5072_D1_2         0x4A
	#define TMC5072_VSTOP_2      0x4B
	#define TMC5072_TZEROWAIT_2  0x4C
	#define TMC5072_XTARGET_2    0x4D
	#define TMC5072_IHOLD_IRUN_2 0x50
	#define TMC5072_VCOOLTHRS_2  0x51
	#define TMC5072_VHIGH_2      0x52
	#define TMC5072_VDCMIN_2     0x53
	#define TMC5072_SWMODE_2     0x54
	#define TMC5072_RAMPSTAT_2   0x55
	#define TMC5072_XLATCH_2     0x56
	#define TMC5072_ENCMODE_2    0x58
	#define TMC5072_XENC_2       0x59
	#define TMC5072_ENC_CONST_2  0x5A
	#define TMC5072_ENC_STATUS_2 0x5B
	#define TMC5072_ENC_LATCH_2  0x5C
 
	#define TMC5072_MSLUT0       0x60
	#define TMC5072_MSLUT1       0x61
	#define TMC5072_MSLUT2       0x62
	#define TMC5072_MSLUT3       0x63
	#define TMC5072_MSLUT4       0x64
	#define TMC5072_MSLUT5       0x65
	#define TMC5072_MSLUT6       0x66
	#define TMC5072_MSLUT7       0x67
	#define TMC5072_MSLUTSEL     0x68
	#define TMC5072_MSLUTSTART   0x69
 
	#define TMC5072_MSCNT_1      0x6A
	#define TMC5072_MSCURACT_1   0x6B
	#define TMC5072_CHOPCONF_1   0x6C
	#define TMC5072_COOLCONF_1   0x6D
	#define TMC5072_DCCTRL_1     0x6E
	#define TMC5072_DRVSTATUS_1  0x6F
 
	#define TMC5072_MSCNT_2      0x7A
	#define TMC5072_MSCURACT_2   0x7B
	#define TMC5072_CHOPCONF_2   0x7C
	#define TMC5072_COOLCONF_2   0x7D
	#define TMC5072_DCCTRL_2     0x7E
	#define TMC5072_DRVSTATUS_2  0x7F
 
	#define TMC5072_PWMCONF_1	  0x10
	#define TMC5072_PWM_STATUS	  0x11
 
	#define TMC5072_RAMPMODE     0x00
	#define TMC5072_XACTUAL      0x01
	#define TMC5072_VACTUAL      0x02
	#define TMC5072_VSTART       0x03
	#define TMC5072_A1           0x04
	#define TMC5072_V1           0x05
	#define TMC5072_AMAX         0x06
	#define TMC5072_VMAX         0x07
	#define TMC5072_DMAX         0x08
	#define TMC5072_D1           0x0A
	#define TMC5072_VSTOP        0x0B
	#define TMC5072_TZEROWAIT    0x0C
	#define TMC5072_XTARGET      0x0D
	#define TMC5072_IHOLD_IRUN   0x10
	#define TMC5072_VCOOLTHRS    0x11
	#define TMC5072_VHIGH        0x12
	#define TMC5072_VDCMIN       0x13
	#define TMC5072_SWMODE       0x14
	#define TMC5072_RAMPSTAT     0x15
	#define TMC5072_XLATCH       0x16
	#define TMC5072_ENCMODE      0x18
	#define TMC5072_XENC         0x19
	#define TMC5072_ENC_CONST    0x1A
	#define TMC5072_ENC_STATUS   0x1B
	#define TMC5072_ENC_LATCH    0x1C
	#define TMC5072_CHOPCONF     0x6C
	#define TMC5072_COOLCONF     0x6D
	#define TMC5072_DRVSTATUS    0x6F
 
	//Motorbits und Write-Bit
	#define TMC5072_MOTOR0       0x20
	#define TMC5072_MOTOR1       0x40
	#define TMC5072_WRITE        0x80
 
	//Rampenmodi (Register TMC5072_RAMPMODE)
	#define TMC5072_MODE_POSITION   0
	#define TMC5072_MODE_VELPOS     1
	#define TMC5072_MODE_VELNEG     2
	#define TMC5072_MODE_HOLD       3
 
	//Endschaltermodusbits (Register TMC5072_SWMODE)
	#define TMC5072_SW_STOPL_ENABLE   0x0001
	#define TMC5072_SW_STOPR_ENABLE   0x0002
	#define TMC5072_SW_STOPL_POLARITY 0x0004
	#define TMC5072_SW_STOPR_POLARITY 0x0008
	#define TMC5072_SW_SWAP_LR        0x0010
	#define TMC5072_SW_LATCH_L_ACT    0x0020
	#define TMC5072_SW_LATCH_L_INACT  0x0040
	#define TMC5072_SW_LATCH_R_ACT    0x0080
	#define TMC5072_SW_LATCH_R_INACT  0x0100
	#define TMC5072_SW_LATCH_ENC      0x0200
	#define TMC5072_SW_SG_STOP        0x0400
	#define TMC5072_SW_SOFTSTOP       0x0800
 
	//Statusbitss (Register TMC5072_RAMPSTAT)
	#define TMC5072_RS_STOPL          0x0001
	#define TMC5072_RS_STOPR          0x0002
	#define TMC5072_RS_LATCHL         0x0004
	#define TMC5072_RS_LATCHR         0x0008
	#define TMC5072_RS_EV_STOPL       0x0010
	#define TMC5072_RS_EV_STOPR       0x0020
	#define TMC5072_RS_EV_STOP_SG     0x0040
	#define TMC5072_RS_EV_POSREACHED  0x0080
	#define TMC5072_RS_VELREACHED     0x0100
	#define TMC5072_RS_POSREACHED     0x0200
	#define TMC5072_RS_VZERO          0x0400
	#define TMC5072_RS_ZEROWAIT       0x0800
	#define TMC5072_RS_SECONDMOVE     0x1000
	#define TMC5072_RS_SG             0x2000
 
	#define MOTOR_ADDR(m) (0x20 << m )
	#define MOTOR_ADDR_DRV(m)  (m << 4)
 
#endif
