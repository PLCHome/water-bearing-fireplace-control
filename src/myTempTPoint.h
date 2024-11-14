#ifndef MYTEMPTPOINT_H_  // Prevent multiple inclusions of this header file
#define MYTEMPTPOINT_H_

#include <Arduino.h>      ///< Includes Arduino framework for basic functionalities
#include <ArduinoJson.h>  ///< Includes ArduinoJson library for JSON handling
#include "myPoint.h"      ///< Includes custom header for the myPoint class
#include "myData.h"       ///< Includes custom header for data handling (presumably)
#include "myServer.h"     ///< Includes custom header for server handling (presumably)

/**
 * @brief A class representing a temperature control point.
 * 
 * This class extends the myPoint class and is used to represent a temperature
 * point with additional functionality for calculating the state based on 
 * temperature values and offsets.
 */
class myTempTPoint : public myPoint  
{
    int tpos = -1;         ///< Position of the first temperature value, initialized to -1 (invalid)
    int tpos2 = -1;        ///< Position of the second temperature value, initialized to -1 (invalid)
    int16_t t2minus = -1;  ///< Offset to subtract from the second temperature, initialized to -1 (invalid)
    int16_t t2plus = -1;   ///< Offset to add to the second temperature, initialized to -1 (invalid)

public:
    /**
     * @brief Construct a new myTempTPoint object using JSON data and a pointer to the next point.
     * 
     * @param json JSON data containing the temperature point configuration.
     * @param next Pointer to the next myPoint object.
     */
    myTempTPoint(JsonVariant json, myPoint* next);
    
    /**
     * @brief Calculate the state of the temperature point.
     * 
     * This method compares the current temperature values with configured thresholds 
     * (t2plus, t2minus) and updates the state of the point (on/off).
     */
    void calcVal() override; 

    /**
     * @brief Get the JSON representation of the current object.
     * 
     * @return A string containing the serialized JSON representation of the object.
     */
    String getJson() override;
};

#endif /* MYTEMPTPOINT_H_ */  // End of header guard
