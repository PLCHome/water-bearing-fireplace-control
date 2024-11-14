#ifndef MYPOINT_H_ ///< Include guard to prevent multiple inclusions of this header
#define MYPOINT_H_

#include <Arduino.h>     ///< Include the core Arduino library for basic functionality
#include <ArduinoJson.h> ///< Include the ArduinoJson library for handling JSON objects

/**
 * @enum ergPoint
 * @brief Enum to represent the possible states of a point.
 *
 * This enum defines three states:
 * - TP_OFF (0): The point is off.
 * - TP_ON (1): The point is on.
 * - TP_ERR (-1): The point is in an error state.
 */
enum ergPoint : int8_t
{
  TP_OFF = 0, ///< The point is off
  TP_ON = 1,  ///< The point is on
  TP_ERR = -1 ///< The point is in an error state
};

/**
 * @class myPoint
 * @brief Class representing a point with a name, id, value, and a linked list structure.
 *
 * This class is used to represent a point, which has an associated name, id, and value.
 * Points are organized in a linked list, and various methods are provided to search for points by id or name.
 */
class myPoint
{
protected:
  String name = "";        ///< Name of the point (initially an empty string)
  int id = -1;             ///< ID of the point (initially -1)
  ergPoint on = TP_ERR;    ///< The state of the point (initially TP_ERR, indicating an error)
  bool calculated = false; ///< Flag to indicate if the value has been calculated
  myPoint *next = NULL;    ///< Pointer to the next myPoint in the linked list (initially NULL)

public:
  /**
   * @brief Constructor to initialize a myPoint object.
   * @param json A JSON object containing the name and id of the point.
   * @param next A pointer to the next myPoint in the linked list.
   */
  myPoint(JsonVariant json, myPoint *next);

  /**
   * @brief Retrieves a point by its ID from the linked list.
   * @param id The ID of the point to search for.
   * @return A pointer to the myPoint with the matching ID, or NULL if not found.
   */
  myPoint *getPoint(int id);

  /**
   * @brief Retrieves a point by its name from the linked list.
   * @param name The name of the point to search for.
   * @return A pointer to the myPoint with the matching name, or NULL if not found.
   */
  myPoint *getPoint(String name);

  /**
   * @brief Retrieves the next point in the linked list.
   * @return A pointer to the next myPoint, or NULL if there is no next point.
   */
  myPoint *getNext();

  /**
   * @brief Unsets the 'calculated' flag for the current point and all subsequent points.
   *
   * This method will recursively set the 'calculated' flag to false for all points in the list.
   */
  void unsetCalculated();

  /**
   * @brief Retrieves the value of the point, calculating it if necessary.
   * @return The state of the point (TP_OFF, TP_ON, or TP_ERR).
   */
  ergPoint getVal();

  /**
   * @brief A pure virtual function to calculate the value of the point.
   *
   * This function must be implemented by any derived classes to define how the point's value is calculated.
   */
  virtual void calcVal() = 0;

  /**
   * @brief A pure virtual function to retrieve the point's JSON representation.
   *
   * This function must be implemented by any derived classes to return the point's JSON data.
   * @return A String containing the JSON representation of the point.
   */
  virtual String getJson() = 0;
};

#endif /* MYPOINT_H_ */ ///< End of the include guard
