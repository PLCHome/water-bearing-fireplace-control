#ifndef MYPOINTS_H_
#define MYPOINTS_H_

#include <Arduino.h>
#include <ArduinoJson.h> ///< Include the ArduinoJson library for handling JSON objects
#include <SPIFFS.h>
#include "myPoint.h"
#include "myTempPoint.h"
#include "myTempTPoint.h"
#include "myLogicPoint.h"
#include "myOutPoint.h"

#define POINTSFILENAME "/config/points.json"

/**
 * @brief Enumeration for different point types.
 */
enum pointTyp : int8_t
{
  PT_TEMP = 0,   /**< Temperature point */
  PT_TEMPT = 1,  /**< Temperature threshold point */
  PT_LOGIC = 2,  /**< Logic point */
  PT_OUT = 3     /**< Output point */
};

/**
 * @brief Class representing a collection of points.
 */
class myPoints
{
  myPoint *first = NULL; /**< Pointer to the first point in the collection. */
  bool changed = false;

public:
  /**
   * @brief Cleans up resources related to points.
   */
  void cleanUp();
  
  void init();

  /**
   * @brief Builds the collection of points.
   */
  void build();

  /**
   * @brief Called every cycle
   */
  void loop();

  /**
   * @brief Calculates and updates the values of points.
   */
  void calcVal(uint32_t change);

  void setChanged();

  String getJSONValue();

  /**
   * @brief Retrieves a point by its ID.
   * 
   * @param id The ID of the point.
   * @return A pointer to the point with the specified ID.
   */
  myPoint *getPoint(int id);

  /**
   * @brief Retrieves a point by its name.
   * 
   * @param name The name of the point.
   * @return A pointer to the point with the specified name.
   */
  myPoint *getPoint(String name);

  /**
   * @brief Gets the value of a point by its ID.
   * 
   * @param id The ID of the point.
   * @return The value associated with the point.
   */
  ergPoint getVal(int id);

  /**
   * @brief Gets the value of a point by its name.
   * 
   * @param name The name of the point.
   * @return The value associated with the point.
   */
  ergPoint getVal(String name);
};

/** External instance of the myPoints class. */
extern myPoints mypoints;

#endif /* MYPOINTS_H_ */
