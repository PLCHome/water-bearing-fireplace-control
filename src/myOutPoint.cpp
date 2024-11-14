#include "myOutPoint.h"  // Einbinden der Header-Datei

/**
 * @brief Konstruktor der Klasse myOutPoint.
 * @param json JSON-Objekt mit den Initialwerten für ida, opos und op.
 * @param next Zeiger auf das nächste Punktobjekt, das von myPoint erbt.
 * Dieser Konstruktor initialisiert die Werte des OutPoints basierend auf den übergebenen JSON-Daten.
 */
myOutPoint::myOutPoint(JsonVariant json, myPoint* next) : myPoint(json, next)  // Aufruf des Konstruktors der Basisklasse
{
    // Überprüfen, ob die entsprechenden Felder im JSON-Objekt existieren und den richtigen Typ haben
    if (json["ida"].is<int>())
    {
        this->ida = json["ida"].as<int>();  // Setzen der ID des OutPoints
    }
    if (json["opos"].is<int>())
    {
        this->opos = json["opos"].as<int>();  // Setzen der Position des OutPoints
    }
    if (json["op"].is<outPoint>())
    {
        this->op = json["op"].as<outPoint>();  // Setzen des Zustands des OutPoints (normal oder negativ)
    }
}

/**
 * @brief Berechnet den aktuellen Wert des OutPoints basierend auf dem Zustand und den Relays.
 * Wenn der berechnete Wert vom aktuellen Zustand abweicht, wird der Relay-Status aktualisiert und die
 * Änderung wird an die Clients weitergegeben.
 */
void myOutPoint::calcVal()
{
    /*
    | `mypoints.getVal(this->ida)` | `this->on` before the run | `this->op` | `this->on` after the run | Description/Logic |
    |------------------------------|---------------------------|------------|--------------------------|-------------------|
    | TP_ERR                       | TP_ERR                    | O_NORM     | TP_ERR                   | No change, error value |
    | TP_ERR                       | TP_ON                     | O_NORM     | TP_ON                    | No change, error value |
    | TP_ERR                       | TP_OFF                    | O_NORM     | TP_OFF                   | No change, error value |
    | TP_ON                        | TP_ERR                    | O_NORM     | TP_ERR                   | No change, error value |
    | TP_ON                        | TP_ON                     | O_NORM     | TP_ON                    | op == O_NORM, no change |
    | TP_ON                        | TP_OFF                    | O_NORM     | TP_OFF                   | op == O_NORM, no change |
    | TP_OFF                       | TP_ERR                    | O_NORM     | TP_ERR                   | No change, error value |
    | TP_OFF                       | TP_ON                     | O_NORM     | TP_ON                    | op == O_NORM, no change |
    | TP_OFF                       | TP_OFF                    | O_NORM     | TP_OFF                   | op == O_NORM, no change |
    | TP_ERR                       | TP_ERR                    | O_NEG      | TP_ERR                   | No change, error value |
    | TP_ERR                       | TP_ON                     | O_NEG      | TP_ON                    | No change, error value |
    | TP_ERR                       | TP_OFF                    | O_NEG      | TP_OFF                   | No change, error value |
    | TP_ON                        | TP_ERR                    | O_NEG      | TP_OFF                   | Inverted value, op == O_NEG |
    | TP_ON                        | TP_ON                     | O_NEG      | TP_OFF                   | op == O_NEG, value is inverted |
    | TP_ON                        | TP_OFF                    | O_NEG      | TP_ON                    | op == O_NEG, value is inverted |
    | TP_OFF                       | TP_ERR                    | O_NEG      | TP_ERR                   | No change, error value |
    | TP_OFF                       | TP_ON                     | O_NEG      | TP_OFF                   | op == O_NEG, value is inverted |
    | TP_OFF                       | TP_OFF                    | O_NEG      | TP_ON                    | op == O_NEG, value is inverted |
    */
    ergPoint calc = TP_ERR;  // Initialisiere die berechnete Ergebnis-Variable als Fehlerwert

    // Überprüfe, ob die Position des OutPoints im gültigen Bereich (Relays) liegt
    if (this->opos >= 0 && this->opos < RELAYS)
    {
        ergPoint pointA = mypoints.getVal(this->ida);  // Hole den Wert des referenzierten Punktes
        if (pointA != TP_ERR)  // Wenn der Wert des referenzierten Punktes gültig ist
        {
            // Berechnung basierend auf dem Zustand des OutPoints
            if (this->op == O_NEG)  // Wenn der Zustand "negativ" ist, toggeln
            {
                calc = (pointA == TP_ON) ? TP_OFF : TP_ON;  // Wenn der Wert an "TP_ON" ist, setze "TP_OFF", sonst umgekehrt
            }
            else
            {
                calc = pointA;  // Ansonsten den Wert des Punktes übernehmen
            }
        }
    }

    // Wenn sich der berechnete Wert vom aktuellen Wert unterscheidet, aktualisiere den Status
    if (calc != this->on)
    {
        this->on = calc;  // Aktualisiere den Wert des OutPoints

        // Wenn der berechnete Wert gültig ist (kein Fehlerwert)
        if (this->on != TP_ERR)
        {
            // Schalte das Relay entsprechend dem berechneten Wert
            relay[this->opos] = (this->on == TP_ON);
        }

        // Erzeuge eine JSON-Darstellung des OutPoints
        String o = getJson();
        Serial.println(o);  // Gebe die JSON-Daten auf der Seriellen Konsole aus
        notifyClients(o);   // Benachrichtige verbundene Clients über die Änderung
    }
}

/**
 * @brief Gibt die JSON-Darstellung des OutPoints zurück.
 * @return JSON-String, der den aktuellen Zustand des OutPoints repräsentiert.
 */
String myOutPoint::getJson()
{
    JsonDocument doc;  // Erstelle ein JSON-Dokument
    doc["id"] = this->id;     // Füge die ID des OutPoints hinzu
    doc["name"] = this->name; // Füge den Namen des OutPoints hinzu
    doc["val"] = this->on;    // Füge den aktuellen Wert (Zustand) des OutPoints hinzu

    String out;
    serializeJson(doc, out);  // Serialisiere das JSON-Dokument zu einem String
    return out;               // Gebe den JSON-String zurück
}
