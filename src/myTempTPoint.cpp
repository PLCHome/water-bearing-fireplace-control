#include "myTempTPoint.h" ///< Includes the header file for myTempTPoint

/**
 * @brief Construct a new myTempTPoint object.
 *
 * This constructor initializes the object from the provided JSON data and
 * a pointer to the next point in the sequence.
 *
 * @param json JSON data containing the temperature point configuration.
 * @param next Pointer to the next myPoint object.
 */
myTempTPoint::myTempTPoint(JsonVariant json, myPoint *next) : myPoint(json, next)
{
    // Check if the JSON contains valid data for tpos and assign it if valid
    if (json["tpos"].is<int>())
    {
        this->tpos = json["tpos"].as<int>();
    }
    // Check if the JSON contains valid data for t2plus and assign it if valid
    if (json["t2plus"].is<int16_t>())
    {
        this->t2plus = json["t2plus"].as<int16_t>();
    }
    // Check if the JSON contains valid data for t2minus and assign it if valid
    if (json["t2minus"].is<int16_t>())
    {
        this->t2minus = json["t2minus"].as<int16_t>();
    }
    // Check if the JSON contains valid data for tpos2 and assign it if valid
    if (json["tpos2"].is<int>())
    {
        this->tpos2 = json["tpos2"].as<int>();
    }
}

/**
 * @brief Calculate the state of the temperature point based on the current temperatures.
 *
 * This method compares the temperature values at the given positions and calculates
 * whether the point should be in the on or off state based on the configured thresholds.
 * If the state changes, the object is serialized into JSON and sent to the clients.
 *
 */
void myTempTPoint::calcVal()
{

    ergPoint calc = this->on; ///< Store the current state (on or off)

    // Check if the temperature positions are valid indices in the tempHoldingReg array
    if (this->tpos >= 0 && this->tpos < TEMPHOLDINGREG && this->tpos2 >= 0 && this->tpos2 < TEMPHOLDINGREG)
    {
        // Retrieve the temperature values from the tempHoldingReg array
        int16_t t = tempHoldingReg[this->tpos];
        int16_t t2 = tempHoldingReg[this->tpos2];

        // Calculate the adjusted temperature values using t2plus and t2minus
        int16_t t2p = t2 + this->t2plus;
        int16_t t2m = t2 + this->t2minus;

        if ((t2m) <= (t2p))
        {
            if (this->on != TP_ON && t >= (t2p))
            {
                calc = TP_ON;
            }
            else if (this->on != TP_OFF && t <= (t2m))
            {
                calc = TP_OFF;
            }
        }
        else
        {
            if (this->on != TP_ON && t <= (t2p))
            {
                calc = TP_ON;
            }
            else if (this->on != TP_OFF && t >= (t2m))
            {
                calc = TP_OFF;
            }
        }
    }

    // If the state has changed, update it and notify clients
    if (calc != this->on)
    {
        this->on = calc;      ///< Update the state (on or off)
        String o = getJson(); ///< Get the JSON representation of the object
        Serial.println(o);    ///< Print the JSON to the serial monitor for debugging
        notifyClients(o);     ///< Notify the clients with the updated JSON data
    }
}

/**
 * @brief Returns the JSON string representation of the object.
 *
 * This method serializes the object data (id, name, and state) into a JSON string
 * for easy transmission or logging.
 *
 * @return A string containing the serialized JSON representation of the object.
 */
String myTempTPoint::getJson()
{
    JsonDocument doc; ///< Create a new JSON document

    // Fill the document with the current object's data
    doc["id"] = this->id;
    doc["name"] = this->name;
    doc["val"] = this->on;

    String out;              ///< Variable to hold the serialized JSON string
    serializeJson(doc, out); ///< Serialize the document into a string
    return out;              ///< Return the JSON string
}
