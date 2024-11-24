#ifndef myMixerPoint_H_
#define myMixerPoint_H_

#include <Arduino.h>
#include <ArduinoJson.h>
#include "myPoint.h"
#include "myServer.h"
#include "myPoints.h"

class Mischer
{
private:
    float Ts;                 // Zieltemperatur
    float T1, T2;             // Aktuelle Temperaturen
    float Vo;                 // Öffnungszustand des Ventils in %
    float Zv;                 // Dauer zum vollständigen Öffnen/Schließen
    float T2min, T2max;       // Minimal- und Maximaltemperatur für T2
    unsigned long Zi;         // Zeitintervall für Regelprüfung
    unsigned long lastMillis; // Letzter Zeitstempel für Timer

    void (*Vauf)(bool); // Callback-Funktion für Ventil öffnen
    void (*Vzu)(bool);  // Callback-Funktion für Ventil schließen

public:
    // Konstruktor
    Mischer(float Ts, float T2min, float T2max, float Zv, float Vo, unsigned long Zi,
            void (*Vauf)(bool), void (*Vzu)(bool));

    // Methode zum Überschreiben der Parameter
    void setParams(float newImp, float newTs, unsigned long newZi, float newZv, float newVo,
                   void (*newVauf)(bool), void (*newVzu)(bool));

    // Loop-Methode für die Regelung
    void loop(float currentT1, bool einFlag);
};

#endif // MISCHER_H
