#include "myTempTPoint.h" ///< Includes the header file for myTempTPoint

/**
 * @brief Construct a new myTempTPoint object.
 *
 * This constructor initializes the object from the provided JSON data and
 * a pointer to the next point in the sequence.
 *
 * @param json JSON data containing the temperature point configuration.
 * @param next Pointer to the next myPoint object.
 */
myTempTPoint::myTempTPoint(JsonVariant json, myPoint *next) : myPoint(json, next)
{
    // Check if the JSON contains valid data for tpos and assign it if valid
    if (json["tpos"].is<int>())
    {
        this->tpos = json["tpos"].as<int>();
    }
    // Check if the JSON contains valid data for t2plus and assign it if valid
    if (json["t2plus"].is<int16_t>())
    {
        this->t2plus = json["t2plus"].as<int16_t>();
    }
    // Check if the JSON contains valid data for t2minus and assign it if valid
    if (json["t2minus"].is<int16_t>())
    {
        this->t2minus = json["t2minus"].as<int16_t>();
    }
    // Check if the JSON contains valid data for tpos2 and assign it if valid
    if (json["tpos2"].is<int>())
    {
        this->tpos2 = json["tpos2"].as<int>();
    }
}

/**
 * @brief Calculate the state of the temperature point based on the current temperatures.
 *
 * This method compares the temperature values at the given positions and calculates
 * whether the point should be in the on or off state based on the configured thresholds.
 * If the state changes, the object is serialized into JSON and sent to the clients.
 *
 * Explanation:
 * Logic:
 *
 * If t2plus >= t2minus, the system compares t to t2p (adjusted t2 with t2plus).
 * If t2plus < t2minus, the system compares t to t2m (adjusted t2 with t2minus).
 * If the temperature t is greater than or equal to t2p (or less than or equal to t2m in the inverted case), the system will turn off (TP_OFF).
 * If the temperature t is less than or equal to t2m (or greater than or equal to t2p in the standard case), the system will turn on (TP_ON).
 * 
*/
void myTempTPoint::calcVal()
{
    /*
     * Table of possible states for `myTempTPoint::calcVal()` method (including positive and negative temperatures):
     *
/*
 * Table of possible states for `myTempTPoint::calcVal()` method (sorted by t2plus, t2minus, t, and t2):
 *
 * | `t` (temperature at tpos) | `t2` (temperature at tpos2) | `t2plus` | `t2minus` | Adjusted `t2p` (t2 + t2plus) | Adjusted `t2m` (t2 + t2minus) | `this->on` before the run | `this->on` after the run | Description/Logic |
 * |--------------------------|-----------------------------|----------|-----------|-----------------------------|-----------------------------|--------------------------|-------------------------|-------------------|
 * | -2150                    | -2200                       | -50      | 50        | -2200                       | -2150                       | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (-2200) |
 * | -2100                    | -2150                       | -50      | 50        | -2150                       | -2100                       | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (-2150) |
 * | -2050                    | -2100                       | -50      | 50        | -2100                       | -2050                       | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (-2100) |
 * | -2000                    | -2050                       | -50      | 50        | -2050                       | -2000                       | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (-2050) |
 * | 2100                     | 2050                        | -50      | 50        | 2050                        | 2000                        | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (20.50°C) |
 * | 2150                     | 2100                        | -50      | 50        | 2100                        | 2000                        | TP_ON                    | TP_OFF                  | Inverted: Turns off if temperature <= t2p (21.00°C) |
 * | 2000                     | 2050                        | -50      | 50        | 2100                        | 2000                        | Any                      | Same as before          | Standard: Turns off if temperature <= t2p (20.00°C) |
 * | 2050                     | 2100                        | -50      | 50        | 2150                        | 2000                        | Any                      | Same as before          | Standard: Turns off if temperature <= t2p (20.50°C) |
 * | -2000                    | -2050                       | 50       | -50       | -2000                       | -2100                       | Any                      | Same as before          | Standard: Turns off if temperature >= t2p (-2000) |
 * | -2050                    | -2100                       | 50       | -50       | -2050                       | -2150                       | Any                      | Same as before          | Standard: No change if temperature == t2p (-2050) |
 * | -2100                    | -2150                       | 50       | -50       | -2100                       | -2200                       | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= t2m (-2150) |
 * | -2150                    | -2200                       | 50       | -50       | -2150                       | -2250                       | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= t2m (-2200) |
 * | 2000                     | 2050                        | 50       | -50       | 2050                        | 2000                        | Any                      | Same as before          | Standard: Turns off if temperature >= t2p (20.50°C) |
 * | 2050                     | 2100                        | 50       | -50       | 2100                        | 2000                        | Any                      | Same as before          | Standard: No change if temperature == t2p (20.50°C) |
 * | 2100                     | 2150                        | 50       | -50       | 2150                        | 2050                        | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= t2m (21.00°C) |
 * | 2150                     | 2200                        | 50       | -50       | 2200                        | 2100                        | TP_OFF                   | TP_ON                   | Standard: Turns on if temperature <= t2m (21.50°C) |
 * | 2100                     | 2150                        | 50       | -50       | 2150                        | 2100                        | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= t2m (21.00°C) |
 * | 2150                     | 2200                        | 50       | -50       | 2200                        | 2150                        | TP_ON                    | TP_OFF                  | Standard: Turns off if temperature >= t2m (21.50°C) |
 * | -2050                    | -2100                       | 50       | 50        | -2050                       | -2000                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2050) |
 * | -2100                    | -2150                       | 50       | 50        | -2100                       | -2000                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2100) |
 * | -2000                    | -2050                       | 50       | 50        | -2000                       | -2100                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2000) |
 * | -2150                    | -2200                       | 50       | 50        | -2150                       | -2200                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2150) |
 * | -2100                    | -2150                       | 50       | 50        | -2100                       | -2200                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2100) |
 * | -2050                    | -2100                       | 50       | 50        | -2050                       | -2150                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2050) |
 * | -2000                    | -2050                       | 50       | 50        | -2000                       | -2100                       | Any                      | Same as before          | Inverted: No change if temperature >= t2m (-2000) |
 * | 2050                     | 2100                        | 50       | 50        | 2100                        | 2150                        | Any                      | Same as before          | Inverted: No change if temperature >= t2m (21.00°C) |
 * | 2000                     | 2050                        | 50       | 50        | 2050                        | 2100                        | Any                      | Same as before          | Inverted: No change if temperature >= t2m (20.50°C) |
 * | 2150                     | 2200                        | 50       | 50        | 2200                        | 2250                        | Any                      | Same as before          | Inverted: No change if temperature >= t2m (21.50°C) |
 * | 2100                     | 2150                        | 50       | 50        | 2150                        | 2200                        | Any                      | Same as before          | Inverted: No change if temperature >= t2m (21.00°C) |
 * | 2000                     | 2050                        | 50       | 50        | 2050                        | 2100                        | Any                      | Same as before          | Standard: No change if temperature >= t2m (20.00°C) |
 * | 2050                     | 2100                        | 50       | 50        | 2100                        | 2150                        | Any                      | Same as before          | Standard: No change if temperature >= t2m (20.50°C) |
 * | 2100                     | 2150                        | 50       | 50        | 2150                        | 2200                        | Any                      | Same as before          | Standard: No change if temperature >= t2m (21.00°C) |
 * | 2150                     | 2200                        | 50       | 50        | 2200                        | 2250                        | Any                      | Same as before          | Standard: No change if temperature >= t2m (21.50°C) |
 */

    ergPoint calc = this->on; ///< Store the current state (on or off)

    // Check if the temperature positions are valid indices in the tempHoldingReg array
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG && this->tpos2 >= 0 && this->tpos2 < TEMPHOLDINGREG)
    {
        // Retrieve the temperature values from the tempHoldingReg array
        int16_t t = tempHoldingReg[this->tpos];
        int16_t t2 = tempHoldingReg[this->tpos2];

        // Calculate the adjusted temperature values using t2plus and t2minus
        int16_t t2p = t2 + this->t2plus;
        int16_t t2m = t2 + this->t2minus;

        // Determine whether to change the state based on the temperature values
        if ((t2p) >= (t2m))
        {
            // If t2plus is greater than or equal to t2minus, check if the state needs to be turned off or on
            if (this->on != TP_OFF && t >= (t2p))
            {
                calc = TP_OFF; ///< Turn off the temperature point
            }
            else if (this->on != TP_ON && t <= (t2m))
            {
                calc = TP_ON; ///< Turn on the temperature point
            }
        }
        else
        {
            // If t2plus is less than t2minus, reverse the logic for turning off or on
            if (this->on != TP_OFF && t <= (t2p))
            {
                calc = TP_OFF; ///< Turn off the temperature point
            }
            else if (this->on != TP_ON && t >= (t2m))
            {
                calc = TP_ON; ///< Turn on the temperature point
            }
        }
    }

    // If the state has changed, update it and notify clients
    if (calc != this->on)
    {
        this->on = calc;      ///< Update the state (on or off)
        String o = getJson(); ///< Get the JSON representation of the object
        Serial.println(o);    ///< Print the JSON to the serial monitor for debugging
        notifyClients(o);     ///< Notify the clients with the updated JSON data
    }
}

/**
 * @brief Returns the JSON string representation of the object.
 *
 * This method serializes the object data (id, name, and state) into a JSON string
 * for easy transmission or logging.
 *
 * @return A string containing the serialized JSON representation of the object.
 */
String myTempTPoint::getJson()
{
    JsonDocument doc; ///< Create a new JSON document

    // Fill the document with the current object's data
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;

    String out;              ///< Variable to hold the serialized JSON string
    serializeJson(doc, out); ///< Serialize the document into a string
    return out;              ///< Return the JSON string
}
