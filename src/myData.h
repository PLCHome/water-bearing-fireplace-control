#ifndef MYDATA_H_
#define MYDATA_H_

// Include necessary libraries and headers
#include "KC868-A8S.h"
#include <ModbusRTUMaster.h>
#include <Adafruit_NeoPixel.h>
#include "PCF8574.h"

// Define constants
#define TEMPHOLDINGREG 24          // Number of temperature holding registers
#define RELAYCOILS 4               // Number of relay coils

// Default pixel color and brightness for the RGB LED
#define INIT_PIXELCOLOR 0xff0000   // Red color (RGB Hex)
#define INIT_PIXELBRIGHTNESS 0     // Minimum brightness

// Enum to track the change type for data
enum change
{
    ch_relay,           // Relay state change
    ch_inputintern,     // Internal input state change
    ch_tempholdingreg   // Temperature holding register change
};

// Enum for internal input states
enum input
{
    in_off = 0x00,       // Input is off
    in_come = 0x03,      // Input has come (activated once)
    in_on = 0x01,        // Input is on (activated)
    in_go = 0x02         // Input is go (inactive but triggered)
};

// Declare external variables for temperature holding registers and relays states
extern int16_t temperatures[TEMPHOLDINGREG];      // Array for temperature holding registers
#define RELAYS PCF8574_OUTPUTS + RELAYCOILS           // Total number of relays
extern bool relays[RELAYS];                           // Array for relays states
extern input inputs[PCF8574_INPUTS];           // Array for internal input states

/**
 * @brief Set the color and brightness for the RGB LED.
 *
 * @param pixelColor The color to set (as a 32-bit RGB value).
 * @param pixelBrightness The brightness of the pixel (0-255).
 */
void setPixel(uint32_t pixelColor, uint8_t pixelBrightness);

/**
 * @brief Set the callback function for handling data changes.
 *
 * @param _dadaChanged The callback function to invoke when data changes.
 */
void setDATAchanged(void (*_dadaChanged)(change));

/**
 * @brief Convert an array of integers to a JSON-formatted string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of integers.
 * @param count The number of elements in the array.
 * @return A JSON string containing the array values.
 */
String jsonArray(String name, int16_t buf[], int count);

/**
 * @brief Convert an array of booleans to a JSON-formatted string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of booleans.
 * @param count The number of elements in the array.
 * @return A JSON string containing the array values.
 */
String jsonArray(String name, bool buf[], int count);

/**
 * @brief Convert an array of input states to a JSON-formatted string.
 *
 * @param name The name of the JSON key.
 * @param buf The array of input states.
 * @param count The number of elements in the array.
 * @return A JSON string containing the array values.
 */
String jsonArray(String name, input buf[], int count);

/**
 * @brief Generate a JSON string representing the internal input states.
 *
 * @return A JSON string for the internal input states.
 */
String jsonInputIntern();

/**
 * @brief Generate a JSON string representing the relays states.
 *
 * @return A JSON string for the relays states.
 */
String jsonRelay();

/**
 * @brief Generate a JSON string representing the temperature holding registers.
 *
 * @return A JSON string for the temperature holding registers.
 */
String jsonTempHoldingReg();

/**
 * @brief Initialize the hardware and communication interfaces.
 * This function sets up Modbus, PCF8574, and RGB LED configurations.
 */
void DATAsetup();

/**
 * @brief Main loop function for handling data reading, processing, and writing.
 * It reads the temperature registers, internal inputs, and writes relays states.
 */
void DATAloop();

#endif /* MYDATA_H_ */
