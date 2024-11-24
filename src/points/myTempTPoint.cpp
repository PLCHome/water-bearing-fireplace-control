#include "myTempTPoint.h"
#include "../data/DataCare.h"

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

/*
 * @brief Calculates the state of the temperature-dependent point based on two temperature inputs.
 *
 * This method updates the state of a temperature point (`on` or `off`) based on two temperature inputs (`t` and `t2`) 
 * and their corresponding thresholds (`t2plus`, `t2minus`). The logic supports both standard and inverted threshold configurations.
 *
 * Logic:
 * - The upper threshold (`t2p`) is calculated as `t2 + t2plus`.
 * - The lower threshold (`t2m`) is calculated as `t2 + t2minus`.
 * - If `t2m <= t2p` (standard logic):
 *     - The point turns **on** if `t >= t2p`.
 *     - The point turns **off** if `t <= t2m`.
 * - If `t2m > t2p` (inverted logic):
 *     - The point turns **on** if `t <= t2p`.
 *     - The point turns **off** if `t >= t2m`.
 *
 * Behavior Table:
 *
 * | t  (Temperature) | t2 (Reference Temp) | t2plus | t2minus | t2p (t2 + t2plus) | t2m (t2 + t2minus) | this->on Before | this->on After | Notes                                   |
 * |------------------|---------------------|--------|---------|-------------------|--------------------|-----------------|----------------|-----------------------------------------|
 * | 2000             | 1500               | 500    | -500    | 2000              | 1000               | TP_OFF          | TP_ON          | t >= t2p, turns on                     |
 * | 2000             | 1500               | 500    | -500    | 2000              | 1000               | TP_ON           | TP_ON          | State remains on                       |
 * | 2000             | 1500               | -500   | 500     | 1000              | 2000               | TP_OFF          | TP_OFF         | t <= t2m, stays off                    |
 * | 2000             | 1500               | -500   | 500     | 1000              | 2000               | TP_ON           | TP_OFF         | t >= t2m, turns off                    |
 * | -2000            | -1500              | 500    | -500    | -1000             | -2000              | TP_OFF          | TP_ON          | Negative temp: t >= t2p, turns on      |
 * | -2000            | -1500              | 500    | -500    | -1000             | -2000              | TP_ON           | TP_ON          | Negative temp: State remains on        |
 * | -2000            | -1500              | -500   | 500     | -2000             | -1000              | TP_OFF          | TP_OFF         | Negative temp: t <= t2m, stays off     |
 * | -2000            | -1500              | -500   | 500     | -2000             | -1000              | TP_ON           | TP_OFF         | Negative temp: t >= t2m, turns off     |
 */

void myTempTPoint::calcVal()
{
    ergPoint calc = this->on; ///< Store the current state (on or off)

    // Check if the temperature positions are valid indices in the tempHoldingReg array
    if (this->tpos >= 0 && this->tpos < datacare.getLenTemeratures() && this->tpos2 >= 0 && this->tpos2 < datacare.getLenTemeratures())
    {
        // Retrieve the temperature values from the tempHoldingReg array
        int16_t t = datacare.getTemeratures()[this->tpos];     ///< Current temperature at tpos
        int16_t t2 = datacare.getTemeratures()[this->tpos2];   ///< Reference temperature at tpos2

        // Calculate the adjusted temperature thresholds
        int16_t t2p = t2 + this->t2plus; ///< Upper threshold: t2 + t2plus
        int16_t t2m = t2 + this->t2minus; ///< Lower threshold: t2 + t2minus

        // Determine the state based on thresholds
        if (t2m <= t2p) ///< Standard logic: t2m is less than or equal to t2p
        {
            if (this->on != TP_ON && t >= t2p)
            {
                calc = TP_ON; ///< Turn on if temperature is greater than or equal to t2p
            }
            else if (this->on != TP_OFF && t <= t2m)
            {
                calc = TP_OFF; ///< Turn off if temperature is less than or equal to t2m
            }
        }
        else ///< Inverted logic: t2m is greater than t2p
        {
            if (this->on != TP_ON && t <= t2p)
            {
                calc = TP_ON; ///< Turn on if temperature is less than or equal to t2p
            }
            else if (this->on != TP_OFF && t >= t2m)
            {
                calc = TP_OFF; ///< Turn off if temperature is greater than or equal to t2m
            }
        }
    }

    // If the calculated state differs from the current state, update and notify
    if (calc != this->on)
    {
        this->on = calc; ///< Update the state to the new value
        mypoints.setChanged();
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
