/****************************************************
  Projekt:  TMC5130 and TMC5072 CRC calculation
 
  Modul:    CRC.cpp
            CRC-Calculation for UART interfacing
 
  Hinweise: Start with CRC-Register=0,
            then call NextCRC for each byte to be sent
            or each by received. Send CRC byte last or
            check received CRC
 
  Datum:    14.6.2011 OK
 
  Geändert: crc counting direction, 17.06.2011, LL, SK
*****************************************************/
 
#include "crc.h"
 
 
uint8 NextCRCSingle(uint8 Crc, uint8 Data, uint8 Gen, uint8 Bit)
{
  uint8 Compare;
 
  Compare=Data<<(7-Bit);
  Compare&=0x80;
 
  if((Crc & 0x80) ^ (Compare))
    return (Crc << 1) ^ Gen;
  else
    return (Crc << 1);
}
 
uint8 NextCRC(uint8 Crc, uint8 Data, uint8 Gen)
{
  uint8 Temp;
  int i;
 
  Temp=Crc;
  for(i=0; i<=7; i++)
  {
    Temp=NextCRCSingle(Temp, Data, Gen, i);
  }
 
  return Temp;
}
