#include "myData.h"
#include "myPoints.h"
#include <ArduinoJson.h>

// Initialize Modbus RTU master instance with the defined serial and RS485 settings.
ModbusRTUMaster modbus(MODBUS_SERIAL, RS485_DEFAULT_DE_PIN);

// Array for storing temperature holding registers.
int16_t temperatures[TEMPHOLDINGREG];

// Arrays for controlling relays and storing their previous state.
bool relays[RELAYS] = {false, false, false, false, false, false, false, false, false, false, false, false};
bool relayLast[RELAYS] = {false, false, false, false, false, false, false, false, true, false, false, false};

// Array for storing input values from PCF8574.
input inputs[PCF8574_INPUTS] = {in_off, in_off, in_off, in_off, in_off, in_off, in_off, in_off};

// Initialize RGB display.
Adafruit_NeoPixel rgb_display = Adafruit_NeoPixel(WS2812_LENGTH, WS2812_PIN, NEO_GRB + NEO_KHZ800);

// Define I2C instances.
TwoWire I2Cone = TwoWire(0);
TwoWire I2Ctwo = TwoWire(1);

// Initialize PCF8574 instances for input and output.
PCF8574 pcf8574_I1(&I2Cone, PCF8574_I1_ADDR, I2C_SDA, I2C_SDL);
PCF8574 pcf8574_R1(&I2Cone, PCF8574_R1_ADDR, I2C_SDA, I2C_SDL);

// Function pointer to handle data changes.
void (*dadaChanged)(change) = NULL;

/**
 * @brief Set RGB LED display color and brightness.
 *
 * @param pixelColor The color of the pixel.
 * @param pixelBrightness The brightness level for the pixel (0-255).
 */
void setPixel(uint32_t pixelColor, uint8_t pixelBrightness)
{
    rgb_display.setPixelColor(WS2812_PIXEL, (pixelColor)); // Set RGB_LED to specified color
    rgb_display.setBrightness(pixelBrightness);
    rgb_display.show();
}

/**
 * @brief Set the function to call when data changes.
 *
 * @param _dadaChanged The function to call when data changes.
 */
void setDATAchanged(void (*_dadaChanged)(change))
{
    dadaChanged = _dadaChanged;
}

/**
 * @brief Initialize the necessary hardware and communication settings.
 * This function sets up Modbus, PCF8574 I/O devices, and the RGB LED.
 */
void DATAsetup()
{
    MODBUS_SERIAL.begin(MODBUS_BUAD, MODBUS_CONFIG, RS485_DEFAULT_RX_PIN, RS485_DEFAULT_TX_PIN);
    modbus.begin(MODBUS_BUAD, MODBUS_CONFIG);

    pinMode(BEEP_PIN, OUTPUT);
    digitalWrite(BEEP_PIN, LOW);

    rgb_display.begin();
    setPixel(INIT_PIXELCOLOR, INIT_PIXELBRIGHTNESS);

    // Set PCF8574 pin modes for output and input.
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

    // Set all PCF8574 output pins to HIGH initially.
    for (int i = 0; i < PCF8574_OUTPUTS; i++)
    {
        pcf8574_R1.digitalWrite(i, HIGH);
    }
}

/**
 * @brief Clean up the Modbus communication.
 * This function ensures that any available data is cleared from the serial buffer.
 */
void ModbusCleanup()
{
    // Clear the Modbus serial buffer.
    while (MODBUS_SERIAL.available() > 0)
    {
        String(MODBUS_SERIAL.read());
    }
}

/**
 * @brief Convert an array of integers to a JSON string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of integers.
 * @param count The number of elements in the array.
 * @return A string containing the JSON representation of the array.
 */
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

/**
 * @brief Convert an array of booleans to a JSON string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of booleans.
 * @param count The number of elements in the array.
 * @return A string containing the JSON representation of the array.
 */
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

/**
 * @brief Convert an array of input values to a JSON string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of input values.
 * @param count The number of elements in the array.
 * @return A string containing the JSON representation of the array.
 */
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

/**
 * @brief Convert the internal input state array to a JSON string.
 * 
 * @return A string containing the JSON representation of the internal input states.
 */
String jsonInputIntern()
{
    return jsonArray("inputintern", inputs, PCF8574_INPUTS);
}

/**
 * @brief Convert the relays state array to a JSON string.
 * 
 * @return A string containing the JSON representation of the relays states.
 */
String jsonRelay()
{
    return jsonArray("relays", relays, RELAYS);
}

/**
 * @brief Convert the temperature holding register values to a JSON string.
 * 
 * @return A string containing the JSON representation of the temperature holding registers.
 */
String jsonTempHoldingReg()
{
    return jsonArray("tempholdingreg", temperatures, TEMPHOLDINGREG);
}

/**
 * @brief Read the temperature values from the Modbus and update the holding registers.
 *
 * @return True if the temperature values were successfully updated, false otherwise.
 */
bool readModbusTemps()
{
    bool result = false;
    ModbusRTUMasterError ModbusErr;
    uint16_t tempHoldingRegRead[TEMPHOLDINGREG];
    ModbusCleanup();
    ModbusErr = modbus.readHoldingRegisters(1, 0x0000, tempHoldingRegRead, TEMPHOLDINGREG);
    if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS)
    {
        if (memcmp(temperatures, tempHoldingRegRead, sizeof(temperatures)) != 0)
        {
            memcpy(temperatures, tempHoldingRegRead, sizeof(temperatures));
            if (dadaChanged)
                (*dadaChanged)(ch_tempholdingreg);
            result = true;
        }
    }
    else
    {
        Serial.println("Read Temps modbus error: " + String(ModbusErr));
    }
    return result;
}

/**
 * @brief Write the relays states to the Modbus relays.
 */
void writeModbusRelays()
{
    ModbusRTUMasterError ModbusErr;
    if (memcmp(&relayLast[PCF8574_OUTPUTS], &relays[PCF8574_OUTPUTS], RELAYCOILS) != 0)
    {
        ModbusCleanup();
        ModbusErr = modbus.writeMultipleCoils(2, 0x0000, &relays[PCF8574_OUTPUTS], RELAYCOILS);
        if (ModbusErr == MODBUS_RTU_MASTER_SUCCESS)
        {
            memcpy(&relayLast[PCF8574_OUTPUTS], &relays[PCF8574_OUTPUTS], RELAYCOILS);
            if (dadaChanged)
                (*dadaChanged)(ch_relay);
            Serial.println("Relays written: " + String(ModbusErr));
        }
        else
        {
            Serial.println("Write relays modbus error: " + String(ModbusErr));
        }
    }
}

/**
 * @brief Read the input states from the PCF8574 and update the input states.
 *
 * @return True if there was a change in the input states, false otherwise.
 */
bool readPCF8574_inputs()
{
    bool changed = false;
    for (int i = 0; i < PCF8574_INPUTS; i++)
    {
        if (pcf8574_I1.digitalRead(i) == LOW)
        {
            if (inputs[i] != in_on)
            {
                if ((inputs[i] == in_come))
                {
                    changed = true;
                    inputs[i] = in_on;
                }
                else if ((inputs[i] != in_come))
                {
                    changed = true;
                    inputs[i] = in_come;
                    relays[i] = !relays[i];
                    relays[PCF8574_OUTPUTS + (i % 4)] = !relays[PCF8574_OUTPUTS + (i % 4)];
                }
            }
        }
        else
        {
            if (inputs[i] != in_off)
            {
                if ((inputs[i] == in_go))
                {
                    changed = true;
                    inputs[i] = in_off;
                }
                else if ((inputs[i] != in_go))
                {
                    changed = true;
                    inputs[i] = in_go;
                }
            }
        }
    }
    if (changed)
    {
        if (dadaChanged)
        {
            (*dadaChanged)(ch_inputintern);
            Serial.println("Input readed: " + jsonArray("iintern", inputs, PCF8574_INPUTS));
        }
    }
    return changed;
}

/**
 * @brief Write the output relays states to the PCF8574.
 */
void writePCF8574_outputs()
{
    for (int i = 0; i < PCF8574_OUTPUTS; i++)
    {
        if (relayLast[i] != relays[i])
        {
            pcf8574_R1.digitalWrite(i, relays[i] ? LOW : HIGH);
            relayLast[i] = relays[i];
            if (dadaChanged)
                (*dadaChanged)(ch_relay);
        }
    }
}

// Timing variables for the main loop.
unsigned long currentTime = millis();
unsigned long previousTime = 0;
const long timeoutTime = 500;

/**
 * @brief Main loop for reading data and updating the system.
 * This function reads Modbus temperatures, PCF8574 inputs, and writes relays.
 */
void DATAloop()
{
    currentTime = millis();
    if (currentTime - previousTime >= timeoutTime)
    {
        previousTime = currentTime;
        if (readModbusTemps() | readPCF8574_inputs())
        {
            mypoints.calcVal();
        }
    }
    writeModbusRelays();
    writePCF8574_outputs();
}
