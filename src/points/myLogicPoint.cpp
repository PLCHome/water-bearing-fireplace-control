#include "points/myLogicPoint.h"
#include "points/myPoints.h"

/**
 * @brief Constructs a myLogicPoint object from JSON data and initializes the
 * logical operation.
 *
 * The constructor sets up the logical point by parsing JSON data to initialize
 * the IDs of the two input points (ida and idb) and the type of logical
 * operation.
 *
 * @param json JSON variant containing initialization data.
 * @param next Pointer to the next myPoint object in the chain.
 */
myLogicPoint::myLogicPoint(JsonVariant json, pointTyp type)
    : myPoint(json, type) {
  // Set the ID of the first point if available in JSON
  if (json["ida"].is<int>()) {
    this->ida = json["ida"].as<int>();
  }

  // Set the logic type (AND, OR, etc.) from JSON, casting to the logicPoint
  // enum
  if (json["logic"].is<int8_t>()) {
    this->logic = json["logic"].as<logicPoint>();
  }

  // Set the ID of the second point if available in JSON
  if (json["idb"].is<int>()) {
    this->idb = json["idb"].as<int>();
  }
}

/**
 * @brief Calculates the value of the logical point based on the specified logic
 * operation and current states.
 *
 * The calcVal method performs the logical operation on the values of two points
 * (identified by ida and idb). If either point is in an error state, the result
 * is set to an error (TP_ERR). The result is then compared to the current state
 * (`on`), and if different, the state is updated and sent to clients.
 */
void myLogicPoint::calcVal() {
  /*
   * Table of possible states for `myLogicPoint::calcVal()` method:
   *
   * | `pointA` | `pointB` | `logic`  | `this->on` before the run | `this->on`
   * after the run | Description/Logic |
   * |----------|----------|----------|---------------------------|--------------------------|-------------------|
   * | TP_ERR   | TP_ERR   | Any      | Any                       | TP_ERR | No
   * valid points, error state | | TP_ERR   | TP_ON    | LO_AND   | Any | TP_ERR
   * | Error state due to invalid pointA | | TP_ERR   | TP_ON    | LO_OR    |
   * Any                       | TP_ERR                   | Error state due to
   * invalid pointA | | TP_ERR   | TP_OFF   | LO_AND   | Any | TP_ERR | Error
   * state due to invalid pointA | | TP_ERR   | TP_OFF   | LO_OR    | Any |
   * TP_ERR                   | Error state due to invalid pointA | | TP_ON    |
   * TP_ERR   | LO_AND   | Any                       | TP_ERR | Error state due
   * to invalid pointB | | TP_ON    | TP_ERR   | LO_OR    | Any | TP_ERR | Error
   * state due to invalid pointB | | TP_OFF   | TP_ERR   | LO_AND   | Any |
   * TP_ERR                   | Error state due to invalid pointB | | TP_OFF   |
   * TP_ERR   | LO_OR    | Any                       | TP_ERR | Error state due
   * to invalid pointB | | TP_ON    | TP_ON    | LO_AND   | Any | TP_ON | Both
   * points are TP_ON, result is TP_ON | | TP_ON    | TP_ON    | LO_OR    | Any
   * | TP_ON                    | Either point is TP_ON, result is TP_ON | |
   * TP_ON    | TP_OFF   | LO_AND   | Any                       | TP_OFF | AND
   * operation: one point is OFF | | TP_ON    | TP_OFF   | LO_OR    | Any |
   * TP_ON                    | OR operation: one point is ON | | TP_OFF   |
   * TP_ERR   | LO_AND   | Any                       | TP_ERR | Error state due
   * to invalid pointB | | TP_OFF   | TP_ERR   | LO_OR    | Any | TP_ERR | Error
   * state due to invalid pointB | | TP_ON    | TP_OFF   | LO_ANDN  | Any |
   * TP_ON                    | ANDN operation: A is ON, B is OFF | | TP_ON    |
   * TP_OFF   | LO_ORN   | Any                       | TP_ON | ORN operation: A
   * is ON, B is OFF | | TP_OFF   | TP_ON    | LO_ANDN  | Any | TP_OFF | ANDN
   * operation: A is OFF, B is ON | | TP_OFF   | TP_ON    | LO_ORN   | Any |
   * TP_ON                    | ORN operation: A is OFF, B is ON |
   */
  ergPoint calc = TP_ERR; // Default result is error state
  ergPoint pointA =
      mypoints.getVal(this->ida); // Get the state of the first point
  ergPoint pointB =
      mypoints.getVal(this->idb); // Get the state of the second point

  // Check if both points are valid (not error states)
  if (pointA != TP_ERR && pointB != TP_ERR) {
    // Perform the logical operation based on the selected logic type
    switch (logic) {
    case LO_AND:
      calc = (pointA == TP_ON && pointB == TP_ON) ? TP_ON : TP_OFF;
      break;
    case LO_OR:
      calc = (pointA == TP_ON || pointB == TP_ON) ? TP_ON : TP_OFF;
      break;
    case LO_ANDN:
      calc = (pointA == TP_ON && pointB == TP_OFF) ? TP_ON : TP_OFF;
      break;
    case LO_ORN:
      calc = (pointA == TP_ON || pointB == TP_OFF) ? TP_ON : TP_OFF;
      break;
    default:
      calc =
          TP_ERR; // If an unsupported logic type is encountered, set to error
    }
  }

  // Update the state if the calculated value differs from the current one
  if (calc != this->on) {
    this->on = calc;
    mypoints.setChanged();
  }
}

void myLogicPoint::getJson(JsonObject &doc) {
  doc["id"] = this->id;
  doc["name"] = this->name;
  doc["val"] = this->on;
  doc["type"] = this->type;
}
