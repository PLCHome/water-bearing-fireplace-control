#ifndef MYTEMPPOINT_H_
#define MYTEMPPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoints.h"
#include "myPoint.h"
#include "myData.h"
#include "myServer.h"

/**
 * @class myTempPoint
 * @brief Class for managing temperature-based data points, inheriting from myPoint.
 * 
 * This class stores and processes temperature thresholds to determine
 * whether the data point is "on" or "off," based on temperature values
 * retrieved from a holding register.
 */
class myTempPoint : public myPoint
{
    int tpos = -1;            /**< Position index in the temperature holding register */
    int16_t toff = 0;         /**< Temperature threshold to turn "off" */
    int16_t ton = 0;          /**< Temperature threshold to turn "on" */

public:
    /**
     * @brief Constructor for myTempPoint.
     * 
     * Initializes the temperature point object using data from JSON and links
     * it to the next myPoint object.
     * 
     * @param json JSON variant with configuration data for initializing the object.
     * @param next Pointer to the next myPoint object in a linked list.
     */
    myTempPoint(JsonVariant json, myPoint* next);

    /**
     * @brief Calculates the current on/off state based on temperature thresholds.
     * 
     * Uses `toff` and `ton` thresholds to determine if the data point should
     * be in "on" or "off" state, depending on the current temperature.
     */
    void calcVal() override; 

    /**
     * @brief Returns the object’s data in JSON format.
     * 
     * Serializes the ID, name, and current state of the object into a JSON string.
     * 
     * @return String JSON representation of the object.
     */
    String getJson() override;
};

#endif /* MYTEMPPOINT_H_ */
