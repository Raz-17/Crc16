#include "Crc16.h"
//Crc 16 library (XModem)
#define CRC_POLY_16  0x9eb2
Crc16 crc(false, false, CRC_POLY_16, 0x0000, 0x0000, 0x8000, 0xFFFF);

void setup()
{
  Serial.begin(9600);
  Serial.println("CRC-16 bit test program");
  Serial.println("=======================");

}

void loop()
{
  /*
    Examples of crc-16 configurations
    Kermit: width=16 poly=0x1021 init=0x0000 refin=true  refout=true  xorout=0x0000 check=0x2189
    Modbus: width=16 poly=0x8005 init=0xffff refin=true  refout=true  xorout=0x0000 check=0x4b37
    XModem: width=16 poly=0x1021 init=0x0000 refin=false refout=false xorout=0x0000 check=0x31c3
    CCITT-False:width=16 poly=0x1021 init=0xffff refin=false refout=false xorout=0x0000 check=0x29b1
    see http://www.lammertbies.nl/comm/info/crc-calculation.html
  */
  //calculate crc incrementally
  byte data[] = {0x1, 0x70, 0x1, 0x69};
  byte msg[6];

  Serial.println("Calculating crc incrementally:");

  crc.clearCrc();
  for (byte i = 0; i < sizeof data; i++)
  {
    Serial.print("byte ");
    Serial.print(i);
    Serial.print(" = 0x");
    Serial.println(data[i],HEX);
    crc.updateCrc(data[i]);
  }
  unsigned short value = crc.getCrc();
  Serial.print("\ncrc = 0x");
  Serial.println(value, HEX);

  Serial.println("\nReference crc:");
  value = calcrc((char*)data, sizeof data);
  Serial.print("crc = 0x");
  Serial.println(value, HEX);


  Serial.println("\nSolving crc incrementally:");
  memcpy(msg, data, sizeof msg);
  msg[4] = value >> 8;
  msg[5] = value & 0xFF;
  crc.clearCrc();
  for (byte i = 0; i < sizeof msg; i++)
  {
    crc.updateCrc(msg[i]);
  }
  value = crc.getCrc();
  Serial.print("\ncrc = 0x");
  Serial.println(value, HEX);

  
  Serial.println("\n\nCalculating crc in a single call");

  //XModem
  value = crc.XModemCrc(data, 0, sizeof data);
  Serial.print("XModem crc = 0x");
  Serial.println(value, HEX);
  memcpy(msg, data, sizeof msg);
  msg[4] = value >> 8;
  msg[5] = value & 0xFF;

  //Reference xmodem
  Serial.println("Reference XModem crc");
  value = calcrcX((char*)data, sizeof data);
  Serial.print("crc = 0x");
  Serial.println(value, HEX);


  Serial.println("\nSolving crc in a single call:");
  value = crc.XModemCrc(msg, 0, sizeof msg);
  Serial.print("crc = 0x");
  Serial.println(value, HEX);


  crc.fastCrc(data, 0, sizeof data, false, false, 0x1021, 0x0000, 0x0000, 0x8000, 0xffff);



  //
  //  //Modbus
  //  value = crc.Modbus(data,0,sizeof data);
  //  Serial.print("Modbus crc = 0x");
  //  Serial.println(value, HEX);
  //
  //  //Mcrf4XX
  //  value = crc.Mcrf4XX(data,0,sizeof data);
  //  Serial.print("Mcrf4XX crc = 0x");
  //  Serial.println(value, HEX);




  while (true);
}
//Check routine taken from
//http://web.mit.edu/6.115/www/miscfiles/amulet/amulet-help/xmodem.htm
int calcrcX(char *ptr, int count)
{
  int  crc;
  char i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (int) * ptr++ << 8;
    i = 8;
    do
    {
      if (crc & 0x8000)
        crc = crc << 1 ^ 0x1021;
      else
        crc = crc << 1;
    } while (--i);
  }
  return (crc);
}

int calcrc(char *ptr, int count)
{
  int  crc;
  char i;
  crc = 0;
  while (--count >= 0)
  {
    crc = crc ^ (int) * ptr++ << 8;
    i = 8;
    do
    {
      if (crc & 0x8000)
        crc = crc << 1 ^ CRC_POLY_16;
      else
        crc = crc << 1;
    } while (--i);
  }
  return (crc);
}
