#include "myPoint.h"

/**
 * @brief Constructor for creating a point from JSON data and linking it to the next point.
 * 
 * This constructor initializes a point with a name and an id from the provided JSON
 * object and links it to the next point in the list.
 * 
 * @param json A JsonVariant containing the point's data.
 * @param next A pointer to the next myPoint in the list.
 */
myPoint::myPoint(JsonVariant json, myPoint* next) : next(next)
{
    // Initialize name from JSON if present
    if (json["name"].is<String>())
    {
        this->name = json["name"].as<String>();
    }
    
    // Initialize id from JSON if present
    if (json["id"].is<int>())
    {
        this->id = json["id"].as<int>();
    }
}

/**
 * @brief Retrieves the point with the specified id from the linked list.
 * 
 * This function searches for a point with the given id. If the current point's id matches
 * the given id, it returns this point. Otherwise, it recursively searches through the
 * subsequent points in the list.
 * 
 * @param id The id of the point to search for.
 * @return myPoint* A pointer to the myPoint with the specified id, or NULL if not found.
 */
myPoint *myPoint::getPoint(int id)
{
    if (this->id == id)
    {
        return this;
    }
    else if (this->next != NULL)
    {
        return this->next->getPoint(id);
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief Retrieves the point with the specified name from the linked list.
 * 
 * This function searches for a point with the given name. If the current point's name matches
 * the provided name, it returns this point. Otherwise, it recursively searches through the
 * subsequent points in the list.
 * 
 * @param name The name of the point to search for.
 * @return myPoint* A pointer to the myPoint with the specified name, or NULL if not found.
 */
myPoint *myPoint::getPoint(String name)
{
    if (this->name.equals(name))
    {
        return this;
    }
    else if (this->next != NULL)
    {
        return this->next->getPoint(name);
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief Retrieves the next point in the linked list.
 * 
 * This function returns a pointer to the next point in the list.
 * 
 * @return myPoint* A pointer to the next myPoint in the list, or NULL if there is no next point.
 */
myPoint *myPoint::getNext()
{
    return this->next;
}

/**
 * @brief Unsets the calculated flag for all points in the linked list.
 * 
 * This function traverses the entire linked list and sets the 'calculated' flag to false
 * for each point, indicating that their values need to be recalculated.
 */
void myPoint::unsetCalculated()
{
    if (this->next != NULL)
    {
        this->next->unsetCalculated();
    }
    this->calculated = false;
}

/**
 * @brief Retrieves the value of the point, calculating it if necessary.
 * 
 * This function returns the value of the point (stored in `on`). If the point has not been
 * calculated yet (based on the `calculated` flag), it calculates the value by calling the 
 * `calcVal()` method and sets the `calculated` flag to true.
 * 
 * @return ergPoint The calculated value of the point.
 */
ergPoint myPoint::getVal()
{
    if (!this->calculated)
    {
        this->calculated = true;
        this->calcVal();
    }
    return this->on;
}


/**
 * @brief Retrieves the value of the point, calculating it if necessary.
 * 
 * call Loop for all points in the linked list.
 * 
 */
void myPoint::loop()
{ }

int myPoint::getId()
{
    return this->id;
}

ergPoint myPoint::getOn()
{
    return this->on;
}
