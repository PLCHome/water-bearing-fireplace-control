#ifndef MYDATA_H_
#define MYDATA_H_

#include "KC868-A8S.h"

#include <ModbusRTUMaster.h>
#include <Adafruit_NeoPixel.h>
#include "PCF8574.h"

#define TEMPHOLDINGREG 24
#define RELAYCOILS 4

#define INIT_PIXELCOLOR 0xff0000
#define INIT_PIXELBRIGHTNESS 0

enum change
{
    ch_relay,
    ch_inputintern,
    ch_tempholdingreg
};

enum input
{
    in_off = 0x00,
    in_come = 0x03,
    in_on = 0x01,
    in_go = 0x02
};

extern int16_t tempHoldingReg[TEMPHOLDINGREG];

#define RELAYS PCF8574_OUTPUTS+RELAYCOILS
extern bool relay[RELAYS];
extern input inputIntern[PCF8574_INPUTS];

void setPixel(uint32_t pixelColor, uint8_t pixelBrightness);
void setDATAchanged(void (*_dadaChanged)(change));
String jsonArray(String name,int16_t buf[], int count);
String jsonArray(String name,bool buf[], int count);
String jsonArray(String name,input buf[], int count);
String jsonInputIntern();
String jsonRelay();
String jsonTempHoldingReg();

void DATAsetup();
void DATAloop();

#endif /* MYDATA_H_ */
