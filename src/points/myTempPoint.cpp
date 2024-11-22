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
}

/**
 * @brief Calculates the current state (on/off) for a temperature point based on thresholds.
 *
 * This method checks the current temperature at a specified position in the holding register 
 * and updates the state (on/off) based on the thresholds `ton` and `toff`.
 * 
 * Normal Logic (toff <= ton):
 *   Positive: Turns on if t >= ton, off if t <= toff.
 *   Negative: Mirrors the same logic with negative thresholds.
 * Inverted Logic (toff > ton):
 *   Positive: Turns on if t <= ton, off if t >= toff.
 *   Negative: Mirrors the same logic with negative thresholds.
 */
void myTempPoint::calcVal()
{
/*
 * Table of possible states for `myTempPoint::calcVal()`:
 *
 * | t (Temperature) | toff (Off Threshold) | ton (On Threshold) | this->on Before | this->on After | Notes                                   |
 * |-----------------|----------------------|--------------------|-----------------|----------------|-----------------------------------------|
 * | t (Temperature) | toff (Off Threshold) | ton (On Threshold) | this->on Before | this->on After | Notes                                   |
 * |-----------------|----------------------|--------------------|-----------------|----------------|-----------------------------------------|
 * | 2000            | 1950                | 2050               | TP_OFF          | TP_OFF         | t <= toff, stays off                   |
 * | 2000            | 1950                | 2050               | TP_ON           | TP_ON          | t >= ton, stays on                     |
 * | 2000            | 2050                | 1950               | TP_OFF          | TP_ON          | Inverted logic: t <= ton, turns on     |
 * | 2000            | 2050                | 1950               | TP_ON           | TP_OFF         | Inverted logic: t >= toff, turns off   |
 * | 2100            | 2050                | 2150               | TP_OFF          | TP_OFF         | Normal logic: t <= toff, stays off     |
 * | 2100            | 2050                | 2150               | TP_ON           | TP_ON          | Normal logic: t >= ton, stays on       |
 * | 1900            | 1950                | 2000               | TP_OFF          | TP_OFF         | Normal logic: t <= toff, stays off     |
 * | 1900            | 1950                | 2000               | TP_ON           | TP_ON          | Normal logic: t >= ton, stays on       |
 * | 2050            | 2000                | 2100               | TP_OFF          | TP_ON          | Normal logic: t >= ton, turns on       |
 * | 2000            | 2050                | 2000               | TP_OFF          | TP_ON          | Inverted logic: t <= ton, turns on     |
 * | 2200            | 2000                | 2100               | TP_ON           | TP_OFF         | Normal logic: t >= toff, turns off     |
 * | -2000           | -1950               | -2050              | TP_OFF          | TP_OFF         | t <= toff, stays off                   |
 * | -2000           | -1950               | -2050              | TP_ON           | TP_ON          | t >= ton, stays on                     |
 * | -2000           | -2050               | -1950              | TP_OFF          | TP_ON          | Inverted logic: t <= ton, turns on     |
 * | -2000           | -2050               | -1950              | TP_ON           | TP_OFF         | Inverted logic: t >= toff, turns off   |
 * | -2100           | -2050               | -2150              | TP_OFF          | TP_OFF         | Normal logic: t <= toff, stays off     |
 * | -2100           | -2050               | -2150              | TP_ON           | TP_ON          | Normal logic: t >= ton, stays on       |
 * | -1900           | -1950               | -2000              | TP_OFF          | TP_OFF         | Normal logic: t <= toff, stays off     |
 * | -1900           | -1950               | -2000              | TP_ON           | TP_ON          | Normal logic: t >= ton, stays on       |
 * | -2050           | -2000               | -2100              | TP_OFF          | TP_ON          | Normal logic: t >= ton, turns on       |
 * | -2000           | -2050               | -2000              | TP_OFF          | TP_ON          | Inverted logic: t <= ton, turns on     |
 * | -2200           | -2000               | -2100              | TP_ON           | TP_OFF         | Normal logic: t >= toff, turns off     |
 */
    ergPoint calc = this->on; // Start with the current state (on/off)

    // Ensure that tpos is within valid bounds of the temperature holding register
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG)
    {
        uint16_t t = temperatures[this->tpos]; // Retrieve the current temperature at index tpos

        // Evaluate the thresholds to determine the state
        if (this->toff <= this->ton) // Normal case: toff is less than or equal to ton
        {
            if (this->on != TP_ON && t >= this->ton) // Turn on if temperature >= ton
            {
                calc = TP_ON;
            }
            else if (this->on != TP_OFF && t <= this->toff) // Turn off if temperature <= toff
            {
                calc = TP_OFF;
            }
        }
        else // Inverted case: toff is greater than ton
        {
            if (this->on != TP_ON && t <= this->ton) // Turn on if temperature <= ton
            {
                calc = TP_ON;
            }
            else if (this->on != TP_OFF && t >= this->toff) // Turn off if temperature >= toff
            {
                calc = TP_OFF;
            }
        }
    }

    // Check if the calculated state differs from the current state
    if (calc != this->on)
    {
        this->on = calc;      // Update the state to the new value
        mypoints.setChanged();
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
