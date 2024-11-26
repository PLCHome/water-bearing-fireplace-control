#include "DataCare.h"

#include "mySetup.h"
#include "MessageDispatcher.h"

#include "beebDOut.h"
#include "modbDOut.h"
#include "modbTemp.h"
#include "pcf8574io.h"
#include "ws2812out.h"

DataCare datacare = DataCare();

DataCare::DataCare()
{
    this->modbus = new Modbus();
    this->i2c = new I2C();
}

void TestDATAloop(void *pvParameters)
{
    datacare.DATAloop(pvParameters);
}

void DataCare::init()
{
    this->modbus->init();
    this->i2c->init();
    createMulti("i2c", "pcf8574", []() -> Datatool *
                { return new pcf8574io(); });
    createMulti("modbus", "temperatures", []() -> Datatool *
                { return new modbTemp(); });
    createMulti("modbus", "outputs", []() -> Datatool *
                { return new modbDOut(); });

    createSingle("beeb", "", []() -> Datatool *
                { return new beebDOut(); });
    createSingle("ws2812led", "", []() -> Datatool *
                { return new ws2812out(); });

    initTempVals();
    initDiVals();
    initDoVals();
    initLedVals();

    Serial.print("DataCare: Tmp: ");
    Serial.print(this->getLenTemeratures());
    Serial.print("DataCare: Di");
    Serial.print(this->getLenInputs());
    Serial.print("DataCare: Do");
    Serial.print(this->getLenOutputs());
    Serial.println();

    xTaskCreate(
        TestDATAloop,
        "DATAloop",         // Name of the task (e.g. for debugging)
        2248,               // Stack size (bytes)
        NULL,               // Parameter to pass
        1,                  // Task priority
        &this->taskDataLoop // Task handle
    );
}

void DataCare::DATAloop(void *pvParameters)
{
    while (true)
    {
        uint32_t change = 0;
        if (this->CurrentLoop++ > loopsToRead)
        {
            CurrentLoop = 0;

            if (this->processTempValues())
            {
                change |= CHANGE_TEMP;
            }
            if (this->processDiValues())
            {
                change |= CHANGE_DI;
            }
        }

        if (this->processDoValues())
        {
            change |= CHANGE_DO;
        }
        if (this->processLedValues())
        {
            change |= CHANGE_LED;
        }
        if (change != 0)
            messagedispatcher.notify(change);

        uint32_t notificationValue = ulTaskNotifyTake(pdTRUE, this->loopTime / portTICK_PERIOD_MS);
    }
}

void DataCare::notifyLoop()
{
    xTaskNotifyGive(this->taskDataLoop);
}


void DataCare::createSingle(String e1, String e2, std::function<Datatool *()> callback)
{
    mysetup->resetSection();
    mysetup->setNextSection(e1);
    if (!e2.isEmpty())
    {
        mysetup->setNextSection(e2);
    }

    Datatool *newtool = callback();
    if (newtool->init(this))
    {
        Serial.println(e1+" in datatools");
        datatools.push_back(newtool);
    }
    else
    {
        delete newtool;
    }
}

void DataCare::createMulti(String e1, String e2, std::function<Datatool *()> callback)
{
    mysetup->resetSection();
    mysetup->setNextSection(e1);
    if (!e2.isEmpty())
    {
        mysetup->setNextSection(e2);
    }
    Serial.print("Is Array? ");
    Serial.print(mysetup->isArrySection());
    if (mysetup->isArrySection())
    {
        uint16_t loop = 0;
        while (mysetup->setArrayElement(loop))
        {
            loop++;
            Serial.println("rufe callback auf!!");
            Datatool *newtool = callback();
            if (newtool->init(this))
            {
                datatools.push_back(newtool);
            }
            else
            {
                delete newtool;
            }
        }
    }
}

void DataCare::initTempVals()
{
    this->lenTemeratures = 0;
    for (const auto &datatool : this->datatools)
    {
        uint16_t a = datatool->getTempVals();
        if (a > 0)
        {
            datatool->setTempValsStart(this->lenTemeratures);
            this->lenTemeratures += a;
        }
    }
    if (this->lenTemeratures > 0)
    {
        this->temeratures = (int16_t *)calloc(this->lenTemeratures, sizeof(int16_t));
        this->lastTemeratures = (int16_t *)calloc(this->lenTemeratures, sizeof(int16_t));
    }
}

void DataCare::initDiVals()
{
    this->lenInputs = 0;
    for (const auto &datatool : this->datatools)
    {
        uint16_t a = datatool->getDIVals();
        if (a > 0)
        {
            datatool->setDIValsStart(this->lenInputs);
            this->lenInputs += a;
        }
    }
    if (this->lenInputs > 0)
    {
        this->inputs = (TA_INPUT *)calloc(this->lenInputs, sizeof(TA_INPUT));
    }
}

void DataCare::initDoVals()
{
    this->lenOutputs = 0;
    for (const auto &datatool : this->datatools)
    {
        uint16_t a = datatool->getDOVals();
        if (a > 0)
        {
            datatool->setDOValsStart(this->lenOutputs);
            this->lenOutputs += a;
        }
    }
    if (this->lenOutputs > 0)
    {
        this->outputs = (bool *)calloc(this->lenOutputs, sizeof(bool));
        this->lastOutputs = (bool *)calloc(this->lenOutputs, sizeof(bool));
    }
}

void DataCare::initLedVals()
{
    this->lenLeds = 0;
    for (const auto &datatool : this->datatools)
    {
        uint16_t a = datatool->getLedVals();
        if (a > 0)
        {
            datatool->setLedValsStart(this->lenLeds);
            this->lenLeds += a;
        }
    }
    if (this->lenLeds > 0)
    {
        this->leds = (uint32_t *)calloc(this->lenLeds, sizeof(uint32_t));
    }
}

bool DataCare::processTempValues()
{
    bool result = false;
    for (const auto &datatool : this->datatools)
    {
        result |= datatool->processTempValues();
    }
    return result;
}

bool DataCare::processDoValues()
{
    bool result = false;
    for (const auto &datatool : this->datatools)
    {
        result |= datatool->processDoValues();
    }
    return result;
}
bool DataCare::processDiValues()
{
    bool result = false;
    for (const auto &datatool : this->datatools)
    {
        result |= datatool->processDiValues();
    }
    return result;
}

bool DataCare::processLedValues()
{
    bool result = false;
    for (const auto &datatool : this->datatools)
    {
        result |= datatool->processLedValues();
    }
    return result;
}

template <typename T>
String DataCare::jsonArray(String name, T buf[], int count)
{
    JsonDocument doc;
    JsonArray data = (!name.isEmpty()) ? doc[name].to<JsonArray>() : doc.to<JsonArray>();

    for (int i = 0; i < count; i++)
    {
        data.add(buf[i]);
    }
    String out;
    serializeJson(doc, out);
    return out;
}

String DataCare::jsonTemeratures(bool obj)
{
    return this->jsonArray(obj?"tempholdingreg":"", this->temeratures, this->lenTemeratures);
}

String DataCare::jsonDI(bool obj)
{
    return this->jsonArray(obj?"inputintern":"", this->inputs, this->lenInputs);
}

String DataCare::jsonDO(bool obj)
{
    return this->jsonArray(obj?"relays":"", this->outputs, this->lenOutputs);
}

Modbus *DataCare::getModbus()
{
    return this->modbus;
}

I2C *DataCare::getI2C()
{
    return this->i2c;
}

int16_t *DataCare::getTemeratures(int16_t pos)
{
    return &this->temeratures[pos];
}

int16_t *DataCare::getLastTemeratures(int16_t pos)
{
    return &this->lastTemeratures[pos];
}

int16_t DataCare::getLenTemeratures()
{
    return this->lenTemeratures;
}

TA_INPUT *DataCare::getInputs(int16_t pos)
{
    return &this->inputs[pos];
}

int16_t DataCare::getLenInputs()
{
    return this->lenInputs;
}

bool *DataCare::getOutputs(int16_t pos)
{
    return &this->outputs[pos];
}

bool *DataCare::getLastOutputs(int16_t pos)
{
    return &this->lastOutputs[pos];
}

int16_t DataCare::getLenOutputs()
{
    return this->lenOutputs;
}

uint32_t *DataCare::getLeds(int16_t pos)
{
    return &this->leds[pos];
}

int16_t DataCare::getLenLeds()
{
    return this->lenLeds;
}
