/*
Es soll in c++ unter Arduino eine Klasse Mischer implenetiert werden.
Die Klasse soll den Volumenstrom vom Mischer in Abhängigkeit von der vorgegebenen Zieltemperatur (Ts) regeln.
Die Klasse soll über Callback-Functionen (Vauf, Vzu) mittels Auf- bzw. Zu Impulsen (Imp) das Mischventil so konfigurieren, daß eine vorher definierte Temperatur (Ts) erreicht und konstant gehalten wird (T1 = Ts).
Die Callback-Functionen (Vauf, Vzu) haben einen boolischen parameter Von der den Ausgang ein oder ausschaltet
Die Klasse soll eine methode Loop anbieten die zyklisch aufgerufen wird, hier wird die Temperatur T1 und T2 sowie ein Ein-Flag übergeben.
Loop soll ebenfalls die Timerfunktion über mills() abbilden.
Die Klasse überprüft alle Zeitintervalle (Zi), ob die Temperatur seit der letzten Messung stetig steigt oder fällt und gibt entsprechend an das Ventil Impulse (Imp) über callback funktionen.
So wird das Ventil weiter geöffnet, wenn T1 < Ts+1C (Hysterese) und Vo < 100% sowie die Temperatur um weniger als 0,3C seit der letzten Messung (Zi) gestiegen ist.
Und entsprechend wird das Ventil weiter geschlossen, wenn T1 > Ts+1C (Hysterese) und Vo > 0% sowie die Temperatur um weniger als 0,3C seit der letzten Messung (Zi) gefallen ist.
Vo ist der Öffnungszustand in %. Über die Herstellerangaben des 3-Wege-Ventils zum vollständigem Öffnen bzw. Schließen (Zv) und der Impulsdauer (Imp), der Öffnungszustand in % (Vo) wird in einer variablen abgelegt.
Über eine zweite Temperatur T2 wird die min und max Temperatur (T2min, T2max) vorgegeben. Das Ventil wird bei Überschreitung der T2max und Unterschreitung der T2min komplett geschloßen.
Wenn das Ein-Flag angibt soll das ventil ebenfalls zugefahren werden.
Alle Variablen und Callbackfunktionen werden dem Konstruktor übergeben und sollen zusetzlich über eine funktion überschreibbar sein: Imp, Ts, Zi, Zv, Vo, Vauf, Vzu.
*/
#include "myMixerPoint.h"

// Konstruktor
Mischer::Mischer(float Ts, float T2min, float T2max, float Zv, float Vo, unsigned long Zi, void (*Vauf)(bool), void (*Vzu)(bool))
    : Ts(Ts), T2min(T2min), T2max(T2max), Zv(Zv), Vo(Vo), Zi(Zi), Vauf(Vauf), Vzu(Vzu)
{
    lastMillis = 0;
}

// Methode zum Überschreiben der Parameter
void Mischer::setParams(float newImp, float newTs, unsigned long newZi, float newZv, float newVo,
                        void (*newVauf)(bool), void (*newVzu)(bool))
{
    Ts = newTs;
    Zi = newZi;
    Zv = newZv;
    Vo = newVo;
    Vauf = newVauf;
    Vzu = newVzu;
}

// Loop-Methode für die Regelung
void Mischer::loop(float currentT1, float currentT2, bool einFlag)
{
    T1 = currentT1;
    T2 = currentT2;

    // Prüfe, ob das Zeitintervall abgelaufen ist
    unsigned long currentMillis = millis();
    if (currentMillis - lastMillis >= Zi)
    {
        lastMillis = currentMillis;

        // Wenn Ein-Flag aus ist oder T2 außerhalb der Grenzwerte ist, Ventil schließen
        if (!einFlag || T2 < T2min || T2 > T2max)
        {
            Vzu(true);
            Vo = 0; // Ventil auf 0 % setzen
            return;
        }

        // Hysterese-Regelung für das Ventil
        if (T1 < Ts + 1.0 && Vo < 100.0 && T1 - currentT1 < 0.3)
        {
            // Öffne Ventil, wenn die Temperatur T1 zu niedrig ist und nicht signifikant steigt
            Vauf(true);
            Vo = min(Vo + (100.0 / Zv), 100.0); // Erhöhe Öffnungszustand in %
        }
        else if (T1 > Ts + 1.0 && Vo > 0.0 && currentT1 - T1 < 0.3)
        {
            // Schließe Ventil, wenn die Temperatur T1 zu hoch ist und nicht signifikant fällt
            Vzu(true);
            Vo = max(Vo - (100.0 / Zv), 0.0); // Reduziere Öffnungszustand in %
        }
        else
        {
            // Falls keine Aktion nötig, Callback-Funktionen deaktivieren
            Vauf(false);
            Vzu(false);
        }
    }
}
