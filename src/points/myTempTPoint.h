#ifndef MYTEMPTPOINT_H_ // Prevent multiple inclusions of this header file
#define MYTEMPTPOINT_H_

#include "points/myPoint.h" ///< Includes custom header for the myPoint class
#include "points/pointtyp.h"
#include <Arduino.h> ///< Includes Arduino framework for basic functionalities
#include <ArduinoJson.h> ///< Includes ArduinoJson library for JSON handling

class myTempTPoint : public myPoint {
  int tpos = -1; ///< Position of the first temperature value, initialized to -1
                 ///< (invalid)
  int tpos2 = -1; ///< Position of the second temperature value, initialized to
                  ///< -1 (invalid)
  int16_t t2minus = -1; ///< Offset to subtract from the second temperature,
                        ///< initialized to -1 (invalid)
  int16_t t2plus = -1; ///< Offset to add to the second temperature, initialized
                       ///< to -1 (invalid)

public:
  myTempTPoint(JsonVariant json, pointTyp typ);
  void calcVal() override;
  void getJson(JsonObject &doc) override;
};

#endif /* MYTEMPTPOINT_H_ */ // End of header guard
