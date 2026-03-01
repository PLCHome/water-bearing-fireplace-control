#include "DS18B20.h"
/*
#include "../../KC868-A8S.h"
#include "../../mySetup.h"

DS18B20::DS18B20() : Hardware(HW_DS18B20) {}

void DS18B20::init() {
  mysetup->resetSection();
  mysetup->setNextSection("ds18b20");
  Hardware::init();
  this->pin = mysetup->getSectionValue<int8_t>("pin", -1);
  this->active &= this->pin > -1;
  if (this->active) {
    this->oneWire = new OneWire(this->pin);
    this->sensors = new DallasTemperature(oneWire);
    this->sensors->begin();
    Serial.println(this->getJsonIDs(true));
  }
}

String deviceAddressToHexString(const DeviceAddress deviceAddress) {
  String hexString = "";
  for (int i = 0; i < 8; i++) {
    if (deviceAddress[i] < 16)
      hexString += "0";                         // Führende Null hinzufügen
    hexString += String(deviceAddress[i], HEX); // Byte in Hex umwandeln
  }
  return hexString;
}

String DS18B20::getJsonIDs(bool obj) {
  JsonDocument doc;
  JsonArray array = obj ? doc["sensors"].to<JsonArray>() : doc.to<JsonArray>();
  if (this->active) {
    while (this->busi)
      delay(1);
    this->busi = true;
    DeviceAddress sensorAddress;
    uint8_t sensorCount = this->sensors->getDeviceCount();
    for (uint8_t i = 0; i < sensorCount; i++) {
      if (this->sensors->getAddress(sensorAddress, i)) {
        array.add(deviceAddressToHexString(sensorAddress));
      }
    }
    this->busi = false;
  }
  String json;
  serializeJson(doc, json);
  return json;
}

DallasTemperature *DS18B20::getSensors() const { return this->sensors; }

void DS18B20::requestTemperatures() {
  if (this->active) {
    while (this->busi)
      delay(1);
    this->busi = true;
    this->sensors->requestTemperatures();
  }
}

void DS18B20::temperaturesFinished() { this->busi = false; }

bool DS18B20::stringToDeviceAddress(const String &str, DeviceAddress &address) {
  if (str.length() != 16)
    return false; // Eine gültige Adresse hat 16 Zeichen

  for (int i = 0; i < 8; i++) {
    String byteString = str.substring(i * 2, (i + 1) * 2);
    address[i] = (uint8_t)strtol(byteString.c_str(), nullptr, 16);
  }
  return true;
}

int16_t DS18B20::getTemp(const DeviceAddress &address) {
  if (this->active) {
    float tempC = sensors->getTempC(address);
    return static_cast<int16_t>(tempC * 100);
  }
  return 0;
}
*/
