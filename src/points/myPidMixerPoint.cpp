#include "myPidMixerPoint.h"
#include "../MessageDispatcher.h"
#include "../data/DataCare.h"
#include "../timer/myTimer.h"
#include "myPoints.h"

// Konstruktor
myPidMixerPoint::myPidMixerPoint(JsonVariant json, pointTyp type)
    : myPoint(json, type) {
  if (json["chkint"].is<int>()) {
    this->checkInterval = json["chkint"].as<int>();
  }
  if (json["imptime"].is<int>()) {
    this->impulseTime = json["imptime"].as<unsigned long>() * 1000;
  }
  if (json["runTime"].is<int>()) {
    this->runTime = json["runTime"].as<unsigned long>() * 1000;
  }
  if (json["ttemp"].is<int>()) {
    this->pidSetpoint = 0.01 * ((double)json["ttemp"].as<int>());
  }
  if (json["nc"].is<bool>()) {
    this->offClosed = json["nc"].as<bool>();
  }

  if (json["Kp"].is<int>()) {
    this->Kp = 0.01 * ((double)json["Kp"].as<int>());
  }
  if (json["Ki"].is<int>()) {
    this->Ki = 0.01 * ((double)json["Ki"].as<int>());
  }
  if (json["Kd"].is<int>()) {
    this->Kd = 0.01 * ((double)json["Kd"].as<int>());
  }

  this->cycleInterval = (unsigned long)this->checkInterval * 1000;
  this->wakeUpTime = 1000;

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

  this->pid = new PID(&this->pidInput, &this->pidOutput, &this->pidSetpoint, Kp,
                      Ki, Kd, DIRECT);
  this->pid->SetSampleTime(this->cycleInterval);
  this->pid->SetOutputLimits(-255, 255);
  this->pid->SetMode(AUTOMATIC);
  mytimer.registerCycle(this);
}

myPidMixerPoint::~myPidMixerPoint() { delete this->pid; }

void myPidMixerPoint::calcVal() {}

void myPidMixerPoint::getJson(JsonObject &doc) {
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
  doc["runTime"] = this->runTime;

  doc["targetTemperature"] = this->targetTemperature;
}

void myPidMixerPoint::doWakeUp() {
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
  if (aktState == MIXER_IS_RESETTING) {
    aktState = MIXER_IS_OFF;
  }
}

void myPidMixerPoint::doReset() {
  aktState = MIXER_IS_RESETTING;
  doClose(this->runTime);
  this->aktPos = 0;
}

bool myPidMixerPoint::doClose(long ms) {
  bool result = true;
  if (this->opopen >= 0 && datacare.getOutputs()[this->opopen]) {
    this->on = TP_ERR;
    result = false; // something is wrong!!!
  } else {
    if (result && this->opclose >= 0 && !datacare.getOutputs()[this->opclose]) {
      mypoints.setChanged();
      datacare.getOutputs()[this->opclose] = true;
      datacare.notifyLoop();
      this->wakeUpTime = ms;
      mytimer.registerWakeUp(this);
      this->on = TP_ON;
    } else {
      this->on = TP_ERR;
      result = false;
    }
  }
  messagedispatcher.notify(CHANGE_MIXER);
  return result;
}

bool myPidMixerPoint::doOpen(long ms) {
  bool result = true;
  if (this->opclose >= 0 && datacare.getOutputs()[this->opclose]) {
    this->on = TP_ERR;
    result = false; // something is wrong!!!
  } else {
    if (result && this->opopen >= 0 && !datacare.getOutputs()[this->opopen]) {
      mypoints.setChanged();
      datacare.getOutputs()[this->opopen] = true;
      datacare.notifyLoop();
      this->wakeUpTime = ms;
      mytimer.registerWakeUp(this);
      this->on = TP_ON;
    } else {
      this->on = TP_ERR;
      result = false;
    }
  }
  messagedispatcher.notify(CHANGE_MIXER);
  return result;
}

void myPidMixerPoint::doCycleIntervall() {
  ergPoint pointON = mypoints.getVal(this->idon);
  if (this->tpos > -1) // get temperatuer
  {
    this->pidInput = 0.01 * (double)datacare.getTemeratures()[this->tpos];
  } else {
    pointON = TP_ERR;
  }

  this->pid->Compute();

  if (aktState == MIXER_IS_OFF && pointON == TP_ON) {
    aktState = MIXER_IS_WORKING;
  }

  if (aktState == MIXER_IS_WORKING && pointON != TP_OFF) {
    doReset();
  }

  if (aktState == MIXER_IS_WORKING && pointON == TP_ON) {
    unsigned long time =
        map(abs(this->pidOutput), 0, 255, 0, this->impulseTime);
    if (time > 100) {
      if (pidOutput > 0) {
        long newTime = this->aktPos + time;
        if (newTime > this->runTime) {
          time = this->runTime - this->aktPos;
          newTime = this->runTime;
        }
        if (time > 0) {
          doOpen(time);
          this->aktPos = newTime;
        }
      } else {
        long newTime = this->aktPos - time;
        if (newTime < 0) {
          time = this->aktPos;
          newTime = 0;
        }
        if (time > 0) {
          doClose(time);
          this->aktPos = newTime;
        }
      }
    }
  }
}
