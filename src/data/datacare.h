#ifndef DATACARE_H_
#define DATACARE_H_

#include <Arduino.h>
#include <vector>
#include "Datatool.h"
#include "Modbus.h"
#include "I2C.h"

// #include <functional>
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

    void createSingle(String e1, String e2, std::function<Datatool *()> callback);
    void createMulti(String e1, String e2, std::function<Datatool *()> callback);
    void initTempVals();
    void initDiVals();
    void initDoVals();
    void initLedVals();

    bool processTempValues();
    bool processDoValues();
    bool processDiValues();
    bool processLedValues();

    template <typename T>
    String jsonArray(String name, T buf[], int count);

public:
    DataCare();
    void init();
    void DATAloop(void *pvParameters);
    
    String jsonTemeratures();
    String jsonDI();
    String jsonDO();

    Modbus *getModbus();
    I2C *getI2C();
    
    int16_t *getTemeratures(int16_t pos = 0);
    int16_t *getLastTemeratures(int16_t pos = 0);
    TA_INPUT *getInputs(int16_t pos = 0);
    bool *getOutputs(int16_t pos = 0);
    bool *getLastOutputs(int16_t pos = 0);
    uint32_t *getLeds(int16_t pos = 0);
    
    int16_t getLenTemeratures();
    int16_t getLenInputs();
    int16_t getLenOutputs();
    int16_t getLenLeds();
};

extern DataCare datacare;

#endif /* DATACARE_H_ */