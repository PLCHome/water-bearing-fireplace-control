#ifndef MYLOGICPOINT_H_
#define MYLOGICPOINT_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "pointtyp.h"

enum logicPoint : int8_t
{
    LO_AND = 0,  /**< Logical AND */
    LO_OR = 1,   /**< Logical OR */
    LO_ANDN = 2, /**< Logical AND NOT */
    LO_ORN = 3,  /**< Logical OR NOT */
    LO_ERR = -1  /**< Error state */
};

class myLogicPoint : public myPoint
{
    int ida = -1;
    int idb = -1;
    logicPoint logic = LO_ERR;

public:
    myLogicPoint(JsonVariant json, pointTyp typ);
    void calcVal() override;
    JsonVariant getJson() override;
};

#endif /* MYLOGICPOINT_H_ */
