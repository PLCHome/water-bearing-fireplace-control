#ifndef MYOUTPOINT_H_  // Verhindert mehrfaches Einbinden der Datei
#define MYOUTPOINT_H_

#include <Arduino.h>        // Basis-Arduino-Bibliothek
#include <ArduinoJson.h>    // JSON-Verarbeitung
#include "myPoints.h"       // Basis für Punktverwaltungsfunktionen
#include "myPoint.h"        // Basis für Punktobjekte
#include "myServer.h"       // Serverkommunikation (z.B. für Benachrichtigungen)

/**
 * @enum outPoint
 * @brief Enum, das die zwei möglichen Zustände des OutPoints beschreibt.
 */
enum outPoint : int8_t
{
    O_NORM = 0,  /**< Normaler Zustand */
    O_NEG = 1    /**< Umgekehrter Zustand */
};

/**
 * @class myOutPoint
 * @brief Diese Klasse repräsentiert einen speziellen Punkt, der von myPoint erbt und zusätzliche Funktionen zur Berechnung und Interaktion mit Relais bietet.
 */
class myOutPoint : public myPoint
{
    int ida = -1;          /**< ID des OutPoints, initialisiert mit -1 */
    int opos = -1;         /**< Position des Relays oder Ausgabepunkts, initialisiert mit -1 */
    outPoint op = O_NORM;  /**< Zustand des OutPoints, Standard ist "normal" (O_NORM) */

public:
    /**
     * @brief Konstruktor für myOutPoint.
     * @param json JSON-Objekt mit den Initialwerten für ida, opos und op.
     * @param next Zeiger auf das nächste Punktobjekt, das von myPoint erbt.
     */
    myOutPoint(JsonVariant json, myPoint* next);

    /**
     * @brief Berechnet den aktuellen Wert des OutPoints basierend auf dem Zustand und den Relays.
     * Diese Methode überschreibt die Methode von myPoint.
     */
    void calcVal() override;

    /**
     * @brief Gibt die JSON-Darstellung des OutPoints zurück.
     * @return JSON-String, der den aktuellen Zustand des OutPoints repräsentiert.
     */
    String getJson() override;
};

#endif /* MYOUTPOINT_H_ */  // Ende des Include Guards
