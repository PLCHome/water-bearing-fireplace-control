#include "myTempPoint.h"

/**
 * @brief Constructor for myTempPoint.
 *
 * This constructor initializes `tpos`, `toff`, and `ton` based on the provided JSON data.
 * If these fields are present in the JSON, their values are assigned to the corresponding
 * member variables. A debug message with the initial values is printed to the serial monitor.
 *
 * @param json JSON variant with initialization data for tpos, toff, and ton.
 * @param next Pointer to the next myPoint object.
 */
myTempPoint::myTempPoint(JsonVariant json, myPoint *next) : myPoint(json, next) // Initializes myTempPoint and calls the base class constructor
{
    // Check if the JSON contains an integer field for tpos and assign it to this->tpos if it exists
    if (json["tpos"].is<int>())
    {
        this->tpos = json["tpos"].as<int>();
    }

    // Check if the JSON contains an int16_t field for toff and assign it to this->toff if it exists
    if (json["toff"].is<int16_t>())
    {
        this->toff = json["toff"].as<int16_t>();
    }

    // Check if the JSON contains an int16_t field for ton and assign it to this->ton if it exists
    if (json["ton"].is<int16_t>())
    {
        this->ton = json["ton"].as<int16_t>();
    }

    // Print debug information, including object ID, name, tpos, toff, and ton
    Serial.println(String("myTempPoint: ") + String(this->id) + " " + String(this->name) + " " + String(this->tpos) + " " + String(this->toff) + " " + String(this->ton));
}

/**
 * @brief Calculates the current on/off state based on temperature thresholds.
 *
 * This method checks the temperature at position `tpos` in the holding register.
 * It compares the temperature against `toff` and `ton` thresholds to determine if
 * the state should switch to "on" or "off." If the state changes, a JSON message
 * is generated and sent to connected clients.
 * Explanation:
 * Standard Case (when toff <= ton):
 * 
 * t <= t_off (2000): The device will turn off (TP_OFF) if the temperature is less than or equal to toff = 2000 (20°C).
 * t >= t_on (2050): The device will turn on (TP_ON) if the temperature is greater than or equal to t_on = 2050 (20.50°C).
 * Inverted Case (when toff > ton):
 * 
 * t >= t_off (2150): The device will turn off (TP_OFF) if the temperature is greater than or equal to toff = 2150 (21.50°C).
 * t <= t_on (2050): The device will turn on (TP_ON) if the temperature is less than or equal to t_on = 2050 (20.50°C).
 * 
 */
void myTempPoint::calcVal()
{
    /*
     * Table of possible states for `myTempPoint::calcVal()` method:
     *
     * | `t` (temperature) | `ton` (on threshold) | `toff` (off threshold) | `this->on` before the run| `this->on` after the run| Description/Logic |
     * |-------------------|----------------------|------------------------|--------------------------|-------------------------|-------------------|
     * | 2000              | 2050                 | 2000                   | Any                      | Same as before          | Standard: Turns off if temperature <= toff (20.00°C) |
     * | 2050              | 2050                 | 2000                   | Any                      | Same as before          | Standard: No change if temperature == toff (20.00°C) |
     * | 2100              | 2050                 | 2000                   | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature >= ton (20.50°C) |
     * | 2150              | 2050                 | 2000                   | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature >= ton (20.50°C) |
     * | 2100              | 2050                 | 2050                   | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= toff (20.50°C) |
     * | 2150              | 2050                 | 2050                   | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= toff (20.50°C) |
     * | 2000              | 2050                 | 2150                   | Any                      | Same as before          | Inverted: No change if temperature <= toff (21.50°C) |
     * | 2050              | 2050                 | 2150                   | Any                      | Same as before          | Inverted: No change if temperature <= toff (21.50°C) |
     * | 2100              | 2050                 | 2150                   | TP_OFF                   | TP_OFF                  | Inverted: No change if temperature <= toff (21.50°C) |
     * | 2150              | 2050                 | 2150                   | TP_OFF                   | TP_OFF                  | Inverted: No change if temperature >= toff (21.50°C) |
     * | 2000              | 2050                 | 2150                   | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature >= toff (21.50°C) |
     * | 2050              | 2050                 | 2150                   | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature >= toff (21.50°C) |
     * | 2100              | 2050                 | 2150                   | TP_ON                    | TP_ON                   | Inverted: No change if temperature <= ton (20.50°C) |
     * | 2150              | 2050                 | 2150                   | TP_ON                    | TP_ON                   | Inverted: No change if temperature <= ton (20.50°C) |
     * |-------------------|----------------------|------------------------|--------------------------|-------------------------|-------------------|
     * | -2000             | -2050                | -2000                  | Any                      | Same as before          | Standard: Turns off if temperature <= toff (-20.00°C) |
     * | -2050             | -2050                | -2000                  | Any                      | Same as before          | Standard: No change if temperature == toff (-20.50°C) |
     * | -2100             | -2050                | -2000                  | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= ton (-21.00°C) |
     * | -2150             | -2050                | -2000                  | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= ton (-21.50°C) |
     * | -2100             | -2050                | -2050                  | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= toff (-21.00°C) |
     * | -2150             | -2050                | -2050                  | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= toff (-21.50°C) |
     * | -2000             | -2050                | -2150                  | Any                      | Same as before          | Inverted: No change if temperature <= toff (-21.50°C) |
     * | -2050             | -2050                | -2150                  | Any                      | Same as before          | Inverted: No change if temperature <= toff (-21.50°C) |
     * | -2100             | -2050                | -2150                  | TP_OFF                   | TP_OFF                  | Inverted: No change if temperature <= toff (-21.50°C) |
     * | -2150             | -2050                | -2150                  | TP_OFF                   | TP_OFF                  | Inverted: No change if temperature >= toff (-21.50°C) |
     * | -2000             | -2050                | -2150                  | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature >= toff (-21.50°C) |
     * | -2050             | -2050                | -2150                  | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature >= toff (-21.50°C) |
     * | -2100             | -2050                | -2150                  | TP_ON                    | TP_ON                   | Inverted: No change if temperature <= ton (-21.00°C) |
     * | -2150             | -2050                | -2150                  | TP_ON                    | TP_ON                   | Inverted: No change if temperature <= ton (-21.50°C) |
     */

    ergPoint calc = this->on; // Start with the current "on" state

    // Ensure that tpos is within valid bounds of the temperature holding register
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG)
    {
        uint16_t t = tempHoldingReg[this->tpos]; // Retrieve temperature from the holding register at index tpos

        // Determine whether to toggle "on" or "off" based on the temperature and threshold settings
        if (this->toff <= this->ton) // Standard case: toff is less than or equal to ton
        {
            if (this->on != TP_OFF && t <= this->toff)
            {
                calc = TP_ON; // Set to "off" if temperature is below or equal to "off" threshold
            }
            else if (this->on != TP_ON && t >= this->ton)
            {
                calc = TP_OFF; // Set to "on" if temperature is equal to or above "on" threshold
            }
        }
        else // Inverted case: toff is greater than ton
        {
            if (this->on != TP_OFF && t >= this->toff)
            {
                calc = TP_ON; // Set to "off" if temperature is above the "off" threshold
            }
            else if (this->on != TP_ON && t <= this->ton)
            {
                calc = TP_OFF; // Set to "on" if temperature is equal to or below "on" threshold
            }
        }
    }

    // Check if the calculated state is different from the current state
    if (calc != this->on)
    {
        this->on = calc;      // Update the state to the new value
        String o = getJson(); // Generate a JSON string of the new state
        Serial.println(o);    // Print the new state to the serial monitor
        notifyClients(o);     // Notify connected clients with the updated state
    }
}

/**
 * @brief Generates a JSON string of the object's current state.
 *
 * Creates a JSON document with `id`, `name`, and `val` representing the
 * object’s ID, name, and current on/off state, respectively. The JSON is
 * serialized to a string and returned.
 *
 * @return String JSON representation of the object.
 */
String myTempPoint::getJson()
{
    JsonDocument doc; ///< Create a new JSON document

    // Fill the document with the current object's data
    doc["id"] = this->id;     // Populate the "id" field with the object's ID
    doc["name"] = this->name; // Populate the "name" field with the object's name
    doc["val"] = this->on;    // Populate the "val" field with the current state (on/off)

    String out;
    serializeJson(doc, out); // Serialize the JSON document to a string format
    return out;              // Return the JSON string
}
