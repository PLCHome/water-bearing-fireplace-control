#include "myPoints.h"

/**
 * @brief Objekt vom Typ myPoints erstellen.
 */
myPoints mypoints = myPoints();

/**
 * @brief Ruft einen Punkt anhand der ID ab.
 * 
 * Diese Funktion durchsucht die Liste der Punkte nach der angegebenen ID.
 * 
 * @param id Die ID des gesuchten Punktes.
 * @return Zeiger auf den gefundenen Punkt oder NULL, wenn der Punkt nicht gefunden wurde.
 */
myPoint *myPoints::getPoint(int id)
{
    if (this->first != NULL) // Überprüft, ob der erste Punkt existiert
    {
        return this->first->getPoint(id); // Ruft den Punkt über die getPoint Methode des ersten Punkts ab
    }
    else
    {
        return NULL; // Gibt NULL zurück, wenn kein Punkt vorhanden
    }
}

/**
 * @brief Ruft einen Punkt anhand des Namens ab.
 * 
 * Diese Funktion durchsucht die Liste der Punkte nach dem angegebenen Namen.
 * 
 * @param name Der Name des gesuchten Punktes.
 * @return Zeiger auf den gefundenen Punkt oder NULL, wenn der Punkt nicht gefunden wurde.
 */
myPoint *myPoints::getPoint(String name)
{
    if (this->first != NULL) // Überprüft, ob der erste Punkt existiert
    {
        return this->first->getPoint(name); // Ruft den Punkt über die getPoint Methode des ersten Punkts ab
    }
    else
    {
        return NULL; // Gibt NULL zurück, wenn kein Punkt vorhanden
    }
}

/**
 * @brief Ruft den Wert eines Punktes anhand der ID ab.
 * 
 * Diese Funktion gibt den Wert des gesuchten Punktes zurück oder einen Fehlerwert,
 * wenn der Punkt nicht gefunden wurde.
 * 
 * @param id Die ID des gesuchten Punktes.
 * @return Der Wert des Punktes oder TP_ERR, wenn der Punkt nicht gefunden wurde.
 */
ergPoint myPoints::getVal(int id)
{
    myPoint *p = this->getPoint(id); // Holt den Punkt mit der angegebenen ID
    if (p != NULL) // Überprüft, ob der Punkt existiert
    {
        return p->getVal(); // Gibt den Wert des Punktes zurück
    }
    else
    {
        return TP_ERR; // Gibt einen Fehlerwert zurück, wenn der Punkt nicht gefunden wurde
    }
}

/**
 * @brief Ruft den Wert eines Punktes anhand des Namens ab.
 * 
 * Diese Funktion gibt den Wert des gesuchten Punktes zurück oder einen Fehlerwert,
 * wenn der Punkt nicht gefunden wurde.
 * 
 * @param name Der Name des gesuchten Punktes.
 * @return Der Wert des Punktes oder TP_ERR, wenn der Punkt nicht gefunden wurde.
 */
ergPoint myPoints::getVal(String name)
{
    myPoint *p = this->getPoint(name); // Holt den Punkt mit dem angegebenen Namen
    if (p != NULL) // Überprüft, ob der Punkt existiert
    {
        return p->getVal(); // Gibt den Wert des Punktes zurück
    }
    else
    {
        return TP_ERR; // Gibt einen Fehlerwert zurück, wenn der Punkt nicht gefunden wurde
    }
}

/**
 * @brief Berechnet die Werte aller Punkte.
 * 
 * Diese Funktion ruft für jeden Punkt in der Liste die getVal()-Methode auf, 
 * um deren Werte zu berechnen und zu aktualisieren.
 */
void myPoints::calcVal()
{
    myPoint *p = this->first; // Startet mit dem ersten Punkt
    this->changed = false;
    if (p != NULL)
    {
        p->unsetCalculated(); // Setzt den berechneten Zustand des ersten Punktes zurück
    }
    while (p != NULL) // Iteriert durch alle Punkte
    {
        p->getVal(); // Ruft den Wert des aktuellen Punktes ab
        p = p->getNext(); // Geht zum nächsten Punkt
    }
    if (this->changed) {
        notifyClients(getJSONValue());
    }
}

/**
 * @brief Bereinigt die Punkteliste.
 * 
 * Diese Funktion löscht den ersten Punkt der Liste und setzt die Liste auf den nächsten Punkt.
 */
void myPoints::cleanUp()
{
    if (this->first != NULL) // Überprüft, ob der erste Punkt existiert
    {
        myPoint *del = this->first; // Der erste Punkt wird zum Löschen gespeichert
        this->first = this->first->getNext(); // Der erste Punkt wird auf den nächsten Punkt gesetzt
        delete del; // Löscht den alten ersten Punkt
    }
}

/**
 * @brief Called every cycle
 */
void myPoints::loop()
{
    myPoint *loop = this->first;
    while (loop != NULL) // Überprüft, ob der erste Punkt existiert
    {
        loop->loop();
        loop = loop->getNext();
    }
}

void myPoints::setChanged()
{
    this->changed = true;
}

String myPoints::getJSONValue()
{
    JsonDocument doc; ///< Create a new JSON document
    JsonObject data = doc["process"].to<JsonObject>();

    // Fill the document with the current object's data
    myPoint *loop = this->first;
    while (loop != NULL) // Überprüft, ob der erste Punkt existiert
    {
        data[String(loop->getId())] = loop->getOn();
        loop->loop();
        loop = loop->getNext();
    }

    String out;              ///< Variable to hold the serialized JSON string
    serializeJson(doc, out); ///< Serialize the document into a string
    return out;              ///< Return the JSON string
}


/**
 * @brief Liest und baut die Punkteliste aus der Datei points.json.
 * 
 * Diese Funktion öffnet die Datei points.json, deserialisiert sie und erstellt 
 * entsprechend dem Punktetyp die jeweiligen Punktobjekte.
 */
void myPoints::build()
{
    this->cleanUp(); // Bereinigt bestehende Punkte
    if (!SPIFFS.begin()) // Versucht, SPIFFS zu initialisieren
    {
        Serial.println("Failed to mount SPIFFS"); // Fehler, wenn SPIFFS nicht gemountet werden konnte
    }
    else
    {
        File file = SPIFFS.open(POINTSFILENAME, FILE_READ); // Öffnet die Datei points.json im Lesemodus
        if (!file)
        {
            Serial.println("There was an error opening the file points.json"); // Fehler beim Öffnen der Datei
            return;
        }
        else
        {
            Serial.println("File points.json opened!"); // Erfolgreiches Öffnen der Datei
            JsonDocument doc; // JSON Dokument erstellen
            DeserializationError error = deserializeJson(doc, file); // JSON deserialisieren
            if (error)
            {
                Serial.println("deserializeJson points.json error..."); // Fehler beim Deserialisieren
            }
            else
            {
                serializeJson(doc, Serial); // Zeigt das deserialisierte JSON zur Überprüfung an
                Serial.println();
                JsonArray array = doc.as<JsonArray>(); // Wandelt das JSON-Dokument in ein Array um
                serializeJson(array, Serial); // Zeigt das Array zur Überprüfung an
                Serial.println();
                Serial.println(String("Prüfe Typ: ") + String(array[2]["type"].as<String>())); // Überprüft und gibt den Typ des dritten Elements aus
                for (int i = array.size() - 1; i >= 0; --i) // Iteriert durch alle Elemente im Array
                {
                    JsonVariant value = array[i];
                    serializeJson(value, Serial); // Zeigt jedes Array-Element an
                    Serial.println();
                    myPoint *next = NULL; // Zeiger auf den nächsten Punkt
                    Serial.println(String("Prüfe Typ: ") + String(value["type"].is<pointTyp>())); // Gibt den Typ des Punktes aus
                    if (value["type"].is<pointTyp>()) // Überprüft, ob der Typ korrekt ist
                    {
                        pointTyp typ = value["type"].as<pointTyp>(); // Holt den Typ des Punktes
                        Serial.println(String("Erzeuge Typ: ") + String(typ)); // Gibt den erstellten Typ aus
                        switch (typ) // Je nach Typ wird der passende Punkt erstellt
                        {
                        case PT_TEMP:
                            next = new myTempPoint(value, this->first); // Erzeugt einen Temperaturpunkt
                            break;
                        case PT_TEMPT:
                            next = new myTempTPoint(value, this->first); // Erzeugt einen weiteren Temperaturpunkt
                            break;
                        case PT_LOGIC:
                            next = new myLogicPoint(value, this->first); // Erzeugt einen Logikpunkt
                            break;
                        case PT_OUT:
                            next = new myOutPoint(value, this->first); // Erzeugt einen Ausgangspunkt
                            break;
                        }
                    }
                    if (next != NULL)
                    {
                        this->first = next; // Setzt den neuen Punkt als ersten Punkt der Liste
                    }
                }
            }
            file.close(); // Schließt die Datei nach der Verarbeitung
        }
    }
}
