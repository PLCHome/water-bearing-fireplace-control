#ifndef DATACARE_H_
#define DATACARE_H_

#include <Arduino.h>
#include <vector>
#include "Datatool.h"
#include "Modbus.h"
#include "I2C.h"
#include "Beeb.h"
#include "Gpio.h"
#include "WS2812.h"
#include "DS18B20.h"

enum TA_INPUT
{
    in_off = 0x00,  // Input is off (inactive)
    in_come = 0x03, // Input has come (activated triggered)
    in_on = 0x01,   // Input is on (activated)
    in_go = 0x02    // Input is go (inactive triggered)
};

class Datatool;

class DataCare
{
private:
    Modbus *modbus;
    I2C *i2c;
    Beeb *beeb;
    Gpio *gpio;
    DS18B20 *ds18b20;
    WS2812 *ws2812;
    std::vector<Datatool *> datatools;
    
    int16_t *temeratures;
    int16_t *lastTemeratures;
    TA_INPUT *inputs;
    bool *outputs;
    bool *lastOutputs;
    uint32_t *leds;
    
    int16_t lenTemeratures;
    int16_t lenInputs;
    int16_t lenOutputs;
    int16_t lenLeds;
    
    TaskHandle_t taskDataLoop;
    
    uint8_t loopTime = 50;
    uint8_t loopsToRead = 10;
    uint8_t CurrentLoop = 0;

    void createIO();
    void initTempVals();
    void initDiVals();
    void initDoVals();
    void initLedVals();

    bool processTempValues();
    bool processDoValues();
    bool processDiValues();
    bool processLedValues();

    template <typename T>
    String jsonArray(String name, T buf[], int count) const;

public:
    DataCare();
    void init();
    void DATAloop(void *pvParameters);
    void notifyLoop();

    String jsonTemeratures(bool obj)  const;
    String jsonDI(bool obj) const;
    String jsonDO(bool obj) const;
    String jsonCounts(bool obj) const;

    Modbus *getModbus() const;
    I2C *getI2C() const;
    Beeb * getBeeb() const;
    Gpio * getGpio() const;
    DS18B20 * getDs18b20() const;
    
    int16_t *getTemeratures(int16_t pos = 0) const;
    int16_t *getLastTemeratures(int16_t pos = 0) const;
    TA_INPUT *getInputs(int16_t pos = 0) const;
    bool *getOutputs(int16_t pos = 0) const;
    bool *getLastOutputs(int16_t pos = 0) const;
    uint32_t *getLeds(int16_t pos = 0) const;
    
    int16_t getLenTemeratures() const;
    int16_t getLenInputs() const;
    int16_t getLenOutputs() const;
    int16_t getLenLeds() const;
};

extern DataCare datacare;

#endif /* DATACARE_H_ */