#define ETH01
#include "myServer.h"
#include <ArduinoJson.h>

#define _ASYNC_WEBSERVER_LOGLEVEL_ 0

#include <AsyncTCP.h>
#include <WiFi.h>

#ifdef ETH01
#include <WebServer_WT32_ETH01.h>
#include <WebServer_WT32_ETH01_Debug.h>
#endif
#include "points/myPoints.h"
#include <DNSServer.h>
#include <ESPAsyncWebServer.h>
#include <MessageDispatcher.h>
#include <Preferences.h>
#include <Update.h>
#include <data/DataCare.h>

/// Filesystem object, set to SPIFFS.
FS *filesystem = &SPIFFS;

/// Web server instance on port 80.
AsyncWebServer server(80);

/// WebSocket instance with URI "/ws".
AsyncWebSocket ws("/ws");

/// File handle for uploading files to the filesystem.
File fsUploadFile;

Preferences preferences;
IPAddress apIP(192, 168, 4, 1); // Statische IP für den Access Point
DNSServer dnsServer;
bool accesspointActive = false;
bool WiFiScanTaskRunning = false;
unsigned long WiFiAptime = 600000;
unsigned long WiFiApstarttime = 0;
bool WiFiActive = true;
bool LANActive = false;

/**
 * @brief Converts bytes to a human-readable string format (B, KB, MB, or GB).
 *
 * @param bytes The number of bytes to format.
 * @return A formatted string representing the size.
 */
String formatBytes(size_t bytes) {
  if (bytes < 1024) {
    return String(bytes) + "B";
  } else if (bytes < (1024 * 1024)) {
    return String(bytes / 1024.0) + "KB";
  } else if (bytes < (1024 * 1024 * 1024)) {
    return String(bytes / 1024.0 / 1024.0) + "MB";
  } else {
    return String(bytes / 1024.0 / 1024.0 / 1024.0) + "GB";
  }
}

/**
 * @brief Determines the MIME type based on the file extension.
 *
 * @param filename The name of the file.
 * @param request The web server request.
 * @return A string representing the MIME type.
 */
String getContentType(String filename, AsyncWebServerRequest *request) {
  if (request->hasArg("download")) {
    return "application/octet-stream";
  } else if (filename.endsWith(".htm") || filename.endsWith(".html")) {
    return "text/html";
  } else if (filename.endsWith(".css")) {
    return "text/css";
  } else if (filename.endsWith(".js")) {
    return "application/javascript";
  } else if (filename.endsWith(".png")) {
    return "image/png";
  } else if (filename.endsWith(".gif")) {
    return "image/gif";
  } else if (filename.endsWith(".jpg")) {
    return "image/jpeg";
  } else if (filename.endsWith(".ico")) {
    return "image/x-icon";
  } else if (filename.endsWith(".xml")) {
    return "text/xml";
  } else if (filename.endsWith(".pdf")) {
    return "application/x-pdf";
  } else if (filename.endsWith(".zip")) {
    return "application/x-zip";
  } else if (filename.endsWith(".gz")) {
    return "application/x-gzip";
  }

  return "text/plain";
}

/**
 * @brief Handles requests for listing files in a specified directory.
 *
 * @param request The web server request.
 */
void handleFileList(AsyncWebServerRequest *request) {
  if (!request->hasArg("dir")) {
    request->send(500, "text/plain", "BAD ARGS");
    return;
  }

  String path = request->arg("dir");
  Serial.println("handleFileList: " + path);

  File root = SPIFFS.open(path);
  path = String();

  JsonDocument doc;
  JsonArray filelist = doc.to<JsonArray>();

  if (root.isDirectory()) {
    File file = root.openNextFile();

    while (file) {
      JsonObject item = filelist.add<JsonObject>();
      item["type"] = (file.isDirectory()) ? "dir" : "file";
      item["path"] = String(file.path());
      item["size"] = file.size();
      file = root.openNextFile();
    }
  }
  root.close();
  String json;
  serializeJson(doc, json);

  Serial.println("handleFileList: " + json);

  request->send(200, "text/json", json);
}

/**
 * @brief Restarts the ESP device upon receiving a request.
 *
 * @param request The web server request.
 */
void handleRestart(AsyncWebServerRequest *request) {
  request->send(200);
  ESP.restart();
}

/**
 * @brief Redirects client requests to the main index page.
 *
 * @param request The web server request.
 */
void handleForwardIndex(AsyncWebServerRequest *request) {
  if (request->client()) {
    IPAddress clientIP = request->client()->getLocalAddress();
    Serial.println("Webserver:" + clientIP.toString());
    if (WiFi.softAPIP() == clientIP) {
      request->redirect("http://" + apIP.toString() + "/wifi.html");
      return;
    }
  }
  request->redirect("/index.html");
}

/**
 * @brief Sends an HTTP 200 OK response.
 *
 * @param request The web server request.
 */
void handleOK(AsyncWebServerRequest *request) { request->send(200); }

/**
 * @brief Deletes a specified file on the filesystem.
 *
 * @param request The web server request.
 */
void handleFileDelete(AsyncWebServerRequest *request) {
  String filename = "";
  if (request->hasParam("filename")) {
    filename = request->getParam("filename")->value();
  }

  if (filename == "") {
    request->send(400, "text/plain", "Filename missing");
    return;
  }

  if (SPIFFS.exists(filename)) {
    if (SPIFFS.remove(filename)) {
      Serial.println("File deleted: " + filename);
      request->send(200, "text/plain", "File deleted: " + filename);
    } else {
      Serial.println("Failed to delete file: " + filename);
      request->send(500, "text/plain", "Error deleting file");
    }
  } else {
    Serial.println("File not found: " + filename);
    request->send(404, "text/plain", "File not found: " + filename);
  }
  request->redirect("/");
}

/**
 * @brief Initializes the filesystem, with formatting if necessary.
 */
void initFS() {
  if (!FileFS.begin(true)) {
    Serial.println(F("Filesystem init failed! Formatting..."));

    if (!FileFS.begin()) {
      while (true) {
        Serial.println(F("Failed to initialize filesystem."));
        delay(5000);
      }
    }
  }
}

/**
 * @brief Manages file upload through web server requests.
 */
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index,
                  uint8_t *data, size_t len, bool final) {
  if (!index) {
    request->_tempFile = SPIFFS.open("/" + filename, "w");
  }

  if (len) {
    request->_tempFile.write(data, len);
  }

  if (final) {
    request->_tempFile.close();
    request->redirect("/");
  }
}

void handleDownload(AsyncWebServerRequest *request) {
  String filename = request->getParam("filename")->value();
  request->send(SPIFFS, filename);
}

void handleSysinfo(AsyncWebServerRequest *request) {
  JsonDocument doc;
  JsonObject utime = doc["uptime"].to<JsonObject>();
  unsigned long uptimeMillis = millis();
  unsigned long uptimeSeconds = uptimeMillis / 1000;
  utime["d"] = uptimeSeconds / 86400;
  utime["h"] = (uptimeSeconds % 86400) / 3600;
  utime["m"] = (uptimeSeconds % 3600) / 60;
  utime["s"] = uptimeSeconds % 60;
  doc["freeheapsize"] = xPortGetFreeHeapSize();
  doc["minimumeverfreeheapsize"] = xPortGetMinimumEverFreeHeapSize();
  doc["heapsize"] = ESP.getHeapSize();
  doc["freeheap"] = ESP.getFreeHeap();
  doc["maxallocheap"] = ESP.getMaxAllocHeap();
  doc["sketchsize"] = ESP.getSketchSize();
  doc["freesketchspace"] = ESP.getFreeSketchSpace();
  doc["flashchipsize"] = ESP.getFlashChipSize();
  doc["flashchipmode"] = ESP.getFlashChipMode();
  doc["flashchipspeed"] = ESP.getFlashChipSpeed();
  doc["spiffstotalbytes"] = SPIFFS.totalBytes();
  doc["spiffsusedbytes"] = SPIFFS.usedBytes();
  doc["chipcores"] = ESP.getChipCores();
  doc["chipmodel"] = ESP.getChipModel();
  doc["chiprevision"] = ESP.getChipRevision();
  doc["cpufreqmhz"] = ESP.getCpuFreqMHz();
  doc["cyclecount"] = ESP.getCycleCount();
  doc["efusemac"] = String(ESP.getEfuseMac(), HEX);

  doc["accesspointActive"] = accesspointActive;
  doc["wifiaptime"] = WiFiAptime;
  doc["lanactive"] = LANActive;
#ifdef ETH01
  doc["lanhostname"] = LANActive ? ETH.getHostname() : "";
#endif
  doc["wifiactive"] = WiFiActive;
  doc["wifimode"] = WiFi.getMode();
  doc["wifihostname"] = WiFi.getHostname();
  if (datacare.getDs18b20()) {
    doc["ds18b20ids"] = datacare.getDs18b20()->getJsonIDs(false);
  }

  String json;
  serializeJson(doc, json);

  request->send(200, "text/json", json);
}

String wifiList() {
  JsonDocument doc;
  JsonArray wifilist = doc["wifilist"].to<JsonArray>();
  int n = WiFi.scanNetworks(); // WLAN-Netzwerke scannen
  for (int i = 0; i < n; ++i) {
    JsonObject item = wifilist.add<JsonObject>();
    item["rssi"] = WiFi.RSSI(i);
    item["ssid"] = WiFi.SSID(i);
    item["bssid"] = WiFi.BSSIDstr(i);
    item["channel"] = WiFi.channel(i);
    item["secure"] = WiFi.encryptionType(i);
  }
  WiFi.scanDelete();

  String json;
  serializeJson(doc, json);
  return json;
}

void handleCounts(AsyncWebServerRequest *request) {
  request->send(200, "text/json", datacare.jsonCounts(true));
}

void setWifi(AsyncWebServerRequest *request) {
  if (request->hasParam("wifi", true)) {
    String ssid = request->getParam("wifi", true)->value();
    String password = request->hasParam("password", true)
                          ? request->getParam("password", true)->value()
                          : "";

    Serial.printf("Versuche mit SSID: %s, Passwort: %s zu verbinden...\n",
                  ssid.c_str(), password.c_str());

    WiFi.begin(ssid.c_str(),
               password.c_str()); // Mit dem angegebenen Netzwerk verbinden
    request->send(200, "text/plain", "Attempting to connect. Please wait...");
    preferences.putString("ssid", ssid);         // Save SSID
    preferences.putString("password", password); // Save Password
  } else {
    request->send(400, "text/plain", "Fehlende Parameter!");
  }
};

void onUpdateStart(AsyncWebServerRequest *request) {
  bool updateSuccess = !Update.hasError();
  AsyncWebServerResponse *response = request->beginResponse(
      200, "text/plain",
      updateSuccess ? "Update erfolgreich!" : "Update fehlgeschlagen!");
  response->addHeader("Connection", "close");
  request->send(response);
  ESP.restart();
}

void onUpdateDo(AsyncWebServerRequest *request, String filename, size_t index,
                uint8_t *data, size_t len, bool final) {
  if (!index) {
    Serial.printf("Update gestartet: %s\n", filename.c_str());
    if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
      Update.printError(Serial);
    }
  }
  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }
  if (final) {
    if (Update.end(true)) {
      Serial.printf("Update erfolgreich: %u Bytes\n", index + len);
    } else {
      Update.printError(Serial);
    }
  }
};

/**
 * @brief Initializes the web server with defined routes and handlers.
 */
void initWebserver() {
  server.on("/list", HTTP_GET, handleFileList);
  server.on("/upload", HTTP_POST, handleOK, handleUpload);
  server.on("/delete-file", HTTP_GET, handleFileDelete);
  server.on("/reboot", HTTP_GET, handleRestart);
  server.on("/setWifi", HTTP_POST, setWifi);
  server.on("/sysinfo", HTTP_GET, handleSysinfo);
  server.on("/count", HTTP_GET, handleCounts);
  server.on("/", HTTP_GET, handleForwardIndex);
  server.on("/index", HTTP_GET, handleForwardIndex);
  server.on("/download", HTTP_GET, handleDownload);
  server.on("/update", HTTP_POST, onUpdateStart, onUpdateDo);
  server.serveStatic("/", SPIFFS, "/www/");
  server.serveStatic("/config/", SPIFFS, "/config/");
  server.onNotFound(handleForwardIndex);
  server.begin();
}

/**
 * @brief Broadcasts sensor readings to all WebSocket clients.
 *
 * @param sensorReadings JSON string with the current sensor readings.
 */
void notifyClients(String sensorReadings) {
  Serial.println(sensorReadings);
  ws.textAll(sensorReadings);
}

/**
 * @brief Called when specific data changes; sends updates to WebSocket clients.
 *
 * @param dataChange Bit value indicating the type of data change.
 */
void webDataChanged(uint32_t dataChange) {
  if ((dataChange & (CHANGE_TEMP + WSGET_DATA)) != 0) {
    notifyClients(datacare.jsonTemeratures(true));
  }
  if ((dataChange & (CHANGE_DI + WSGET_DATA)) != 0) {
    notifyClients(datacare.jsonDI(true));
  }
  if ((dataChange & (CHANGE_DO + WSGET_DATA)) != 0) {
    notifyClients(datacare.jsonDO(true));
  }
  // if (dataChange & (CHANGE_LED|WSGET_DATA) != 0) {
  //     notifyClients(datacare.jsonLED());
  // }
  if ((dataChange & (CHANGE_POINTS + WSGET_DATA)) != 0) {
    notifyClients(mypoints.getJSONValue(true));
  }
  if ((dataChange & (CHANGE_MIXER)) != 0) {
    notifyClients(mypoints.getJSONValueMixer());
  }
  if ((dataChange & (TIME_TRIGGER + WSGET_DATA)) != 0) {
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);
    char buf[32];
    JsonDocument doc;
    JsonObject time = doc["time"].to<JsonObject>();
    if (timeinfo.tm_sec == 0 || (dataChange & (WSGET_DATA))) {
      time["dst"] = timeinfo.tm_isdst;
      time["wd"] = timeinfo.tm_wday;
      time["yd"] = timeinfo.tm_yday;
      time["d"] = timeinfo.tm_mday;
      time["mo"] = timeinfo.tm_mon + 1;
      time["y"] = timeinfo.tm_year + 1900;
      time["h"] = timeinfo.tm_hour;
      time["mi"] = timeinfo.tm_min;
    }
    time["s"] = timeinfo.tm_sec;
    String json;
    serializeJson(doc, json);
    notifyClients(json);
  }
}

void sendWifiList() {
  if (!WiFiScanTaskRunning) {
    WiFiScanTaskRunning = true;
    xTaskCreate(
        [](void *) { // vTaskDelay(1000 / portTICK_PERIOD_MS);
          notifyClients(wifiList());
          WiFiScanTaskRunning = false;
          vTaskDelete(NULL);
        },
        "WiFiScanTask", 2048, NULL, 1, NULL);
  }
}

/**
 * @brief Handles WebSocket messages and triggers updates or actions.
 */
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo *)arg;
  if (info->final && info->index == 0 && info->len == len &&
      info->opcode == WS_TEXT) {
    data[len] = 0;
    String message = (char *)data;
    Serial.println("WS message: " + message);
    if (strchr((char *)data, '{') == NULL) {
      if (strcmp((char *)data, "getReadings") == 0) {
        messagedispatcher.notify(WSGET_DATA);
      } else if (strcmp((char *)data, "reloadPoints") == 0) {
        mypoints.build();
      } else if (strcmp((char *)data, "reboot") == 0) {
        ESP.restart();
      } else if (strcmp((char *)data, "wifilist") == 0) {
        sendWifiList();
      } else if (strcmp((char *)data, "ds18b20ids") == 0) {
        if (datacare.getDs18b20()) {
          notifyClients(datacare.getDs18b20()->getJsonIDs(true));
        }
      }
    } else {
      JsonDocument doc;
      deserializeJson(doc, (char *)data);
      JsonObject root = doc.as<JsonObject>();
      for (JsonPair kv : root) {
        String key = String(kv.key().c_str());
        if (key.startsWith("relays")) {
          int pos = key.substring(6).toInt();
          if (pos >= 0 && pos < datacare.getLenOutputs()) {
            datacare.getOutputs()[pos] = kv.value().as<bool>();
          }
        }
      }
    }
  }
}

/**
 * @brief Manages WebSocket events like connection, disconnection, and data
 * reception.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
             AwsEventType type, void *arg, uint8_t *data, size_t len) {
  switch (type) {
  case WS_EVT_CONNECT:
    Serial.printf("WebSocket client #%u connected from %s\n", client->id(),
                  client->remoteIP().toString().c_str());
    break;
  case WS_EVT_DISCONNECT:
    Serial.printf("WebSocket client #%u disconnected\n", client->id());
    break;
  case WS_EVT_DATA:
    handleWebSocketMessage(arg, data, len);
    break;
  case WS_EVT_PONG:
  case WS_EVT_ERROR:
    break;
  }
}

/**
 * @brief Initializes the WebSocket service.
 */
void initWebSocket() {
  ws.onEvent(onEvent);
  server.addHandler(&ws);
}

// Dynamische SSID und Passwort basierend auf Seriennummer
String getDynamicSSID() {
  uint64_t chipId = ESP.getEfuseMac(); // Seriennummer (MAC-Adresse)
  // Extrahiere die letzten 6 Ziffern der MAC-Adresse in Hex-Format
  String lastSix = String((uint32_t)chipId, HEX).substring(2);
  return "myESP_" + lastSix;
}

String getDynamicPassword() {
  uint64_t chipId = ESP.getEfuseMac(); // Seriennummer (MAC-Adresse)
  String lastSix = String((uint32_t)chipId, HEX).substring(2);
  return "config_" + lastSix;
}

void startAccessPoint() {
  // Access Point starten
  WiFi.persistent(false);
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP);
  WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
  WiFi.softAP(getDynamicSSID().c_str(), getDynamicPassword().c_str());

  // DNS-Server starten, alle Domains auf apIP umleiten
  dnsServer.setErrorReplyCode(DNSReplyCode::NoError);
  WiFiApstarttime = 0;
  accesspointActive = true;
  dnsServer.start(53, "*", apIP);

  Serial.print("AP gestartet. IP-Adresse: ");
  Serial.println(WiFi.softAPIP());
}

void stopAccessPoint() {
  // Access Point starten
  WiFi.persistent(false);
  WiFi.softAPdisconnect(true);
  dnsServer.stop();
  WiFiApstarttime = 0;
  accesspointActive = false;
  WiFi.mode(WIFI_OFF);
  Serial.print("AP gestoppt.");
}

/**
 * @brief Configures and initializes the web server, WebSocket, and Ethernet.
 */
void WEBsetup() {
  messagedispatcher.registerCallback(webDataChanged);

  mysetup->resetSection();
#ifdef ETH01
  mysetup->setNextSection("lan");
  WT32_ETH01_onEvent();
  String lanHostname = mysetup->getSectionValue<String>("hostname", "");
  if (!lanHostname.isEmpty()) {
    ETH.setHostname(mysetup->cstrPersists(lanHostname));
  }
  LANActive = mysetup->getSectionValue<bool>("active", false);
  if (LANActive) {
    Serial.print("\nStarting WebServer on " + String(ARDUINO_BOARD));
    Serial.println(WEBSERVER_WT32_ETH01_VERSION);

    ETH.begin(
        ETH_PHY_ADDR, mysetup->getSectionValue<int>("phy_power", ETH_PHY_POWER),
        mysetup->getSectionValue<int>("phy_mdc", ETH_PHY_MDC),
        mysetup->getSectionValue<int>("phy_mdio", ETH_PHY_MDIO),
        mysetup->getSectionValue<eth_phy_type_t>("type", ETH_PHY_TYPE),
        mysetup->getSectionValue<eth_clock_mode_t>("clk_mode", ETH_CLK_MODE));
  }
#endif
  mysetup->resetSection();
  mysetup->setNextSection("wifi");
  WiFiActive = mysetup->getSectionValue<bool>("active", WiFiActive);
  WiFiAptime =
      mysetup->getSectionValue<uint16_t>("aptime", WiFiAptime / 1000) * 1000;
  String wifiHostname = mysetup->getSectionValue<String>("hostname", "");
  if (!wifiHostname.isEmpty()) {
    WiFi.setHostname(mysetup->cstrPersists(wifiHostname));
  }
  if (WiFiActive) {
    preferences.begin("wifi", false);

    // Versuche, mit gespeicherten Zugangsdaten zu verbinden
    String savedSSID = preferences.getString("ssid", "");
    String savedPassword = preferences.getString("password", "");

    if (savedSSID != "" && savedPassword != "") {
      Serial.println("Trying to connect with saved network...");
      WiFi.mode(WIFI_STA);
      WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

      unsigned long startAttemptTime = millis();
      while (WiFi.status() != WL_CONNECTED) {
        if (millis() - startAttemptTime > 10000) { // 10 Sekunden Timeout
          break;
        }
        delay(100);
      }
      if (WiFi.status() == WL_CONNECTED) {
        Serial.println("Successfully connected to saved network!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());
      } else {
        Serial.println("Failed to connect to saved network.");
      }
    }
    if (WiFi.status() != WL_CONNECTED && WiFiAptime > 0) {
      Serial.println("Starting Access Point...");
      String ssid = getDynamicSSID();
      String password = getDynamicPassword();
      startAccessPoint();
    }
  }

  initWebSocket();
  initWebserver();

  mysetup->resetSection();
  mysetup->setNextSection("ntp");

  String ntpServer = mysetup->getSectionValue<String>("server", "pool.ntp.org");
  String timeZone =
      mysetup->getSectionValue<String>("tz", "CET-1CEST,M3.5.0,M10.5.0/3");

  configTzTime(mysetup->cstrPersists(timeZone),
               mysetup->cstrPersists(ntpServer));
}

/**
 * @brief Manages WebSocket clients, cleaning up inactive clients.
 */
void WEBloop() {
  ws.cleanupClients();
  if (accesspointActive) {
    dnsServer.processNextRequest();
    if (WiFiApstarttime == 0)
      WiFiApstarttime = millis();
    if (millis() - WiFiApstarttime > (WiFiAptime * 1000)) {
      Serial.println("Ap wegen Zeitablauf gestoppt:" + String(WiFiAptime) +
                     " < " + String(millis() - WiFiApstarttime));
      stopAccessPoint();
    }
  }
}
