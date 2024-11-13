#include "myData.h"
#include "myPoints.h"
#include <ArduinoJson.h>

ModbusRTUMaster modbus(MODBUS_SERIAL, RS485_DEFAULT_DE_PIN);

int16_t tempHoldingReg[TEMPHOLDINGREG];

bool relay[RELAYS] = {false, false, false, false, false, false, false, false, false, false, false, false};
bool relayLast[RELAYS] = {false, false, false, false, false, false, false, false, true, false, false, false};

input inputIntern[PCF8574_INPUTS] = {in_off, in_off, in_off, in_off, in_off, in_off, in_off, in_off};

Adafruit_NeoPixel rgb_display = Adafruit_NeoPixel(WS2812_LENGTH, WS2812_PIN, NEO_GRB + NEO_KHZ800);

TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);
// set I2C of A8S
PCF8574 pcf8574_I1(&I2Cone, PCF8574_I1_ADDR, I2C_SDA, I2C_SDL);
PCF8574 pcf8574_R1(&I2Cone, PCF8574_R1_ADDR, I2C_SDA, I2C_SDL);

void (*dadaChanged)(change) = NULL;

void setPixel(uint32_t pixelColor, uint8_t pixelBrightness)
{
    rgb_display.setPixelColor(WS2812_PIXEL, (pixelColor)); // Set RGB_LED to RED color
    rgb_display.setBrightness(pixelBrightness);
    rgb_display.show();
}

void setDATAchanged(void (*_dadaChanged)(change))
{
    dadaChanged = _dadaChanged;
}

void DATAsetup()
{
    MODBUS_SERIAL.begin(MODBUS_BUAD, MODBUS_CONFIG, RS485_DEFAULT_RX_PIN, RS485_DEFAULT_TX_PIN);
    modbus.begin(MODBUS_BUAD, MODBUS_CONFIG);

    pinMode(BEEP_PIN, OUTPUT);
    digitalWrite(BEEP_PIN, LOW);

    rgb_display.begin();
    setPixel(INIT_PIXELCOLOR, INIT_PIXELBRIGHTNESS);
    // Set PCF8574 pinMode to OUTPUT
    for (int i = 0; i < PCF8574_OUTPUTS; i++)
    {
        pcf8574_R1.pinMode(i, OUTPUT);
    }
    for (int i = 0; i < PCF8574_INPUTS; i++)
    {
        pcf8574_I1.pinMode(i, INPUT);
    }

    pcf8574_R1.begin();
    pcf8574_I1.begin();

    for (int i = 0; i < PCF8574_OUTPUTS; i++)
    {
        pcf8574_R1.digitalWrite(i, HIGH);
    }
}

void ModbusCleanup()
{
    // Serial.println(String(MODBUS_SERIAL.baudRate())+":"+MODBUS_SERIAL.available());
    while (MODBUS_SERIAL.available() > 0)
    {
        String(MODBUS_SERIAL.read());
    }
}

String jsonArray(String name, int16_t buf[], int count)
{
    JsonDocument doc;
    JsonArray data = doc[name].to<JsonArray>();
    for (int i = 0; i < count; i++)
    {
        data.add(buf[i]);
    }
    String out;
    serializeJson(doc, out);
    return out;
}

String jsonArray(String name, bool buf[], int count)
{
    JsonDocument doc;
    JsonArray data = doc[name].to<JsonArray>();
    for (int i = 0; i < count; i++)
    {
        data.add(buf[i]);
    }
    String out;
    serializeJson(doc, out);
    return out;
}

String jsonArray(String name, input buf[], int count)
{
    JsonDocument doc;
    JsonArray data = doc[name].to<JsonArray>();
    for (int i = 0; i < count; i++)
    {
        data.add(buf[i]);
    }
    String out;
    serializeJson(doc, out);
    return out;
}

String jsonInputIntern()
{
    return jsonArray("inputintern", inputIntern, PCF8574_INPUTS);
}

String jsonRelay()
{
    return jsonArray("relay", relay, RELAYS);
}

String jsonTempHoldingReg()
{
    return jsonArray("tempholdingreg", tempHoldingReg, TEMPHOLDINGREG);
}

bool readModbusTemps()
{
    bool result = false;
    ModbusRTUMasterError ModbusErr;
    uint16_t tempHoldingRegRead[TEMPHOLDINGREG];
    ModbusCleanup();
    ModbusErr = modbus.readHoldingRegisters(1, 0x0000, tempHoldingRegRead, TEMPHOLDINGREG);
    if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS)
    {
        if (memcmp(tempHoldingReg, tempHoldingRegRead, sizeof(tempHoldingReg)) != 0)
        {
            memcpy(tempHoldingReg, tempHoldingRegRead, sizeof(tempHoldingReg));
            if (dadaChanged)
                (*dadaChanged)(ch_tempholdingreg);
            Serial.println("Modbus readed: " + jsonArray("temps", tempHoldingReg, TEMPHOLDINGREG));
            result = true;
        }
    }
    else
    {
        Serial.println("Read Temps modbus error: " + String(ModbusErr));
    }
    return result;
}

void writeModbusRelays()
{
    ModbusRTUMasterError ModbusErr;
    if (memcmp(&relayLast[PCF8574_OUTPUTS], &relay[PCF8574_OUTPUTS], RELAYCOILS) != 0)
    {
        ModbusCleanup();
        ModbusErr = modbus.writeMultipleCoils(2, 0x0000, &relay[PCF8574_OUTPUTS], RELAYCOILS);
        if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS)
        {
            memcpy(&relayLast[PCF8574_OUTPUTS], &relay[PCF8574_OUTPUTS], RELAYCOILS);
            if (dadaChanged)
                (*dadaChanged)(ch_relay);
            Serial.println("Relays written: " + String(ModbusErr));
        }
        else
        {
            Serial.println("Wride relays modbus error: " + String(ModbusErr));
        }
    }
}

bool readPCF8574_inputs()
{
    bool changed = false;
    for (int i = 0; i < PCF8574_INPUTS; i++)
    {
        if (pcf8574_I1.digitalRead(i) == LOW)
        {
            if (inputIntern[i] != in_on)
            {
                if ((inputIntern[i] == in_come))
                {
                    // geschaltet
                    changed = true;
                    inputIntern[i] = in_on;
                }
                else if ((inputIntern[i] != in_come))
                {
                    // geschaltet
                    changed = true;
                    inputIntern[i] = in_come;
                    relay[i] = !relay[i];
                    relay[PCF8574_OUTPUTS + (i % 4)] = !relay[PCF8574_OUTPUTS + (i % 4)];
                }
            }
        }
        else
        {
            if (inputIntern[i] != in_off)
            {
                if ((inputIntern[i] == in_go))
                {
                    // geschaltet
                    changed = true;
                    inputIntern[i] = in_off;
                }
                else if ((inputIntern[i] != in_go))
                {
                    // geschaltet
                    changed = true;
                    inputIntern[i] = in_go;
                }
            }
        }
    }
    if (changed)
    {
        if (dadaChanged)
        {
            (*dadaChanged)(ch_inputintern);
            Serial.println("Input readed: " + jsonArray("iintern", inputIntern, PCF8574_INPUTS));
        }
    }
    return changed;
}

void writePCF8574_outputs()
{
    for (int i = 0; i < PCF8574_OUTPUTS; i++)
    {
        if (relayLast[i] != relay[i])
        {
            pcf8574_R1.digitalWrite(i, relay[i] ? LOW : HIGH);
            relayLast[i] = relay[i];
            if (dadaChanged)
                (*dadaChanged)(ch_relay);
        }
    }
}

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 500;

void DATAloop()
{
    currentTime = millis();
    if (currentTime - previousTime >= timeoutTime)
    {
        previousTime = currentTime;
        if (readModbusTemps() |
            readPCF8574_inputs())
        {
            mypoints.calcVal();
        }
    }
    writeModbusRelays();
    writePCF8574_outputs();
}