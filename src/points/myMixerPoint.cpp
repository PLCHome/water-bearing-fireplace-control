/*
Es soll in c++ unter Arduino eine Klasse Mischer auf ESP Wroom 32 implenetiert
werden. Die Klasse soll den Volumenstrom vom Mischer in Abhängigkeit von der
vorgegebenen Zieltemperatur (Ts) regeln. Die Klasse soll über Functionen (Vauf,
Vzu) mittels Auf- bzw. Zu Impulsen (Imp) das Mischventil so konfigurieren, daß
eine vorher definierte Temperatur (Ts) erreicht und konstant gehalten wird (T1 =
Ts). Die Functionen (Vauf, Vzu) haben keinen parameter da sie den Ausgang für
die imp-Zeit einschalten. Die Klasse soll FreeRTOS xTask benutzen nicht loop.
Die Klasse in header und implementation getrennt sein
Die Klasse überprüft alle Zeitintervalle (Zi), ob die Temperatur seit der
letzten Messung stetig steigt oder fällt und gibt entsprechend an das Ventil
Impulse (Imp) über callback funktionen. So wird das Ventil weiter geöffnet, wenn
T1 < Ts+1C (1C Hysterese) und Vo < 100% sowie die Temperatur um weniger als 0,3C
seit der letzten Messung (Zi) gestiegen ist. Und entsprechend wird das Ventil
weiter geschlossen, wenn T1 > Ts+1C (1C Hysterese) und Vo > 0% sowie die
Temperatur um weniger als 0,3C seit der letzten Messung (Zi) gefallen ist. Vo
ist der Öffnungszustand in %. Die Herstellerangaben des 3-Wege-Ventils Impulse
zum vollständigem Öffnen bzw. Schließen (Zv) und der Impulsdauer (Imp), der
Öffnungszustand in % (Vo) wird in einer variablen abgelegt. Wenn das Ein-Flag
angibt soll das ventil ebenfalls zugefahren werden. Alle Variablen und
Callbackfunktionen werden dem Konstruktor übergeben und sollen zusetzlich über
eine funktion überschreibbar sein: Imp, Ts, Zi, Zv, Vo, Vauf, Vzu. Nach der
Impulsdauer (Imp) muss die Callbackroutine wieder mitfals aufgerufen werden.
Schreibe den Code bitte auf englisch.
*/
#include "myMixerPoint.h"
#include "../MessageDispatcher.h"
#include "../timer/myTimer.h"
#include "myPoints.h"
#include <data/DataCare.h>

// Konstruktor
myMixerPoint::myMixerPoint(JsonVariant json, pointTyp type)
    : myPoint(json, type) {
  if (json["chkint"].is<int>()) {
    this->checkInterval = json["chkint"].as<int>();
  }
  if (json["imptime"].is<int>()) {
    this->impulseTime = json["imptime"].as<int>();
  }
  if (json["impmax"].is<int>()) {
    this->pulsesToOpen = json["impmax"].as<int>();
  }
  if (json["ttemp"].is<int>()) {
    this->targetTemperature = json["ttemp"].as<int>();
  }
  if (json["nc"].is<bool>()) {
    this->offClosed = json["nc"].as<bool>();
  }

  this->wakeUpTime = (unsigned long) this->impulseTime * 1000;
  this->cycleInterval = (unsigned long) this->checkInterval * 1000;
  this->closing = this->pulsesToOpen;

  if (json["idon"].is<int>()) {
    this->idon = json["idon"].as<int>();
  }

  if (json["tpos"].is<int>()) {
    this->tpos = json["tpos"].as<int>();
  }
  if (this->tpos < 0 || this->tpos >= datacare.getLenTemeratures()) {
    this->tpos = -1;
  }
  if (json["opclose"].is<int>()) {
    this->opclose = json["opclose"].as<int>();
  }
  if (this->opclose < 0 || this->opclose >= datacare.getLenOutputs()) {
    this->opclose = -1;
  }
  if (json["opopen"].is<int>()) {
    this->opopen = json["opopen"].as<int>();
  }
  if (this->opopen < 0 || this->opopen >= datacare.getLenOutputs()) {
    this->opopen = -1;
  }

  mytimer.registerCycle(this);
}

myMixerPoint::~myMixerPoint(){
  mytimer.unregisterWakeUp(this);
  mytimer.unregisterCycle(this);
}

void myMixerPoint::calcVal() {}

void myMixerPoint::getJson(JsonObject &doc) {
  doc["id"] = this->id;
  doc["name"] = this->name;
  doc["val"] = this->on;
  doc["type"] = this->type;

  doc["idon"] = this->idon;
  doc["tpos"] = this->tpos;
  doc["opclose"] = this->opclose;
  doc["opopen"] = this->opopen;

  doc["mc"] = this->offClosed;
  doc["checkInterval"] = this->checkInterval;
  doc["impulseTime"] = this->impulseTime;
  doc["pulsesToOpen"] = this->pulsesToOpen;
  doc["currentPulse"] = this->currentPulse;
  doc["lastTemperature"] = this->lastTemperature;
  doc["hysteresis"] = this->hysteresis;
  doc["delta"] = this->delta;
  doc["targetTemperature"] = this->targetTemperature;
  doc["prozent"] = this->prozent;
  doc["closing"] = this->closing;
}

void myMixerPoint::doWakeUp() {
  this->on = TP_OFF;
  if (this->opclose >= 0) {
    mypoints.setChanged();
    datacare.getOutputs()[this->opclose] = false;
  } else
    this->on = TP_ERR;
  if (this->opopen >= 0) {
    mypoints.setChanged();
    datacare.getOutputs()[this->opopen] = false;
    this->on = TP_OFF;
  } else
    this->on = TP_ERR;
}

bool myMixerPoint::doClose() {
  messagedispatcher.notify(CHANGE_MIXER);
  if (this->opopen >= 0 && datacare.getOutputs()[this->opopen]) {
    this->on = TP_ERR;
    return false; // something is wrong!!!
  }
  if (this->opclose >= 0 && !datacare.getOutputs()[this->opclose]) {
    mypoints.setChanged();
    datacare.getOutputs()[this->opclose] = true;
    datacare.notifyLoop();
    mytimer.registerWakeUp(this);
    this->on = TP_ON;
    return true;
  } else
    this->on = TP_ERR;
  return false;
}

bool myMixerPoint::doOpen() {
  messagedispatcher.notify(CHANGE_MIXER);
  if (this->opclose >= 0 && datacare.getOutputs()[this->opclose]) {
    this->on = TP_ERR;
    return false; // something is wrong!!!
  }
  if (this->opopen >= 0 && !datacare.getOutputs()[this->opopen]) {
    mypoints.setChanged();
    datacare.getOutputs()[this->opopen] = true;
    datacare.notifyLoop();
    mytimer.registerWakeUp(this);
    this->on = TP_ON;
    return true;
  } else
    this->on = TP_ERR;
  return false;
}

void myMixerPoint::doCycleIntervall() {
  uint16_t t;
  ergPoint pointON = mypoints.getVal(this->idon);
  if (this->tpos > -1) // get temperatuer
  {
    t = datacare.getTemeratures()[this->tpos];
  } else {
    pointON = TP_ERR;
  }
  if (closing > 0) // still closing
  {
    setcycleInterval(true);
    if (this->offClosed) {
      if (doClose())
        closing--;
      prozent = (closing * 100 / pulsesToOpen);
    } else {
      if (doOpen())
        closing--;
      prozent = ((pulsesToOpen - closing) * 100 / pulsesToOpen);
    }
  } else if (closing == 0 && pointON == TP_ON) // still closed and switched on
  {
    setcycleInterval(false);
    if (this->offClosed) {
      currentPulse = 0;
      closing = -1;
    } else {
      currentPulse = pulsesToOpen;
      closing = -1;
    }
    prozent = (currentPulse * 100 / pulsesToOpen);
  } else if (closing == -1 &&
             pointON != TP_ON) // still on and switched off of err
  {
    setcycleInterval(true);
    closing = pulsesToOpen;
    prozent = 100;
  }

  if (closing == -1) // atill on
  {
    setcycleInterval(false);
    int16_t aktDelta = (t - lastTemperature);
    if ((t < (targetTemperature - hysteresis)) &&
        (currentPulse < pulsesToOpen) && (aktDelta < delta)) {
      if (doOpen())
        currentPulse++;
    }
    if (t > (targetTemperature + hysteresis) && (currentPulse > 0) &&
        ((aktDelta * -1) < delta)) {
      if (doClose())
        currentPulse--;
    }
    prozent = (currentPulse * 100 / pulsesToOpen);
  }
  // store temperature
  lastTemperature = t;
}

void myMixerPoint::setcycleInterval(bool reset) {
  this->cycleInterval =
      (reset && (this->impulseTime * 1500 < this->checkInterval * 1000))
          ? this->impulseTime * 1500
          : this->checkInterval * 1000;
}
