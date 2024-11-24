#ifndef MYLOGICPOINT_H_
#define MYLOGICPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myServer.h"
#include "myPoints.h"

/**
 * @brief Enumeration for logical operations.
 */
enum logicPoint : int8_t
{
    LO_AND = 0,   /**< Logical AND */
    LO_OR = 1,    /**< Logical OR */
    LO_ANDN = 2,  /**< Logical AND NOT */
    LO_ORN = 3,   /**< Logical OR NOT */
    LO_ERR = -1   /**< Error state */
};

/**
 * @brief A class representing a logical point that performs operations based on the states of two other points.
 * 
 * The myLogicPoint class extends myPoint and implements logical operations
 * (AND, OR, AND NOT, OR NOT) between two other points. It is initialized with JSON data
 * and can calculate its logical value based on the operation type and states of two input points.
 */
class myLogicPoint : public myPoint
{
    int ida = -1;                  /**< ID of the first point for the logical operation */
    int idb = -1;                  /**< ID of the second point for the logical operation */
    logicPoint logic = LO_ERR;     /**< Type of logical operation */

public:
    /**
     * @brief Constructs a myLogicPoint object with initialization from JSON data.
     * 
     * @param json JSON variant with initialization data for the logical point.
     * @param next Pointer to the next myPoint object in a linked list or chain.
     */
    myLogicPoint(JsonVariant json, myPoint *next);

    /**
     * @brief Calculates the current value based on the logic operation and the states of the two points.
     */
    void calcVal() override;

    /**
     * @brief Returns a JSON representation of the logical point's current state.
     * 
     * @return String JSON string with the current state information.
     */
    String getJson() override;
};

#endif /* MYLOGICPOINT_H_ */
