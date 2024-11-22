#include "myServer.h"
#include <ArduinoJson.h>

#define _ASYNC_WEBSERVER_LOGLEVEL_ 0

#include <WiFi.h>
#include <AsyncTCP.h>
#include <WebServer_WT32_ETH01.h>
#include <WebServer_WT32_ETH01_Debug.h>
#include <ESPAsyncWebServer.h>
#include "points/myPoints.h"
#include <DNSServer.h>
#include <Preferences.h>

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


/**
 * @brief Converts bytes to a human-readable string format (B, KB, MB, or GB).
 *
 * @param bytes The number of bytes to format.
 * @return A formatted string representing the size.
 */
String formatBytes(size_t bytes)
{
    if (bytes < 1024)
    {
        return String(bytes) + "B";
    }
    else if (bytes < (1024 * 1024))
    {
        return String(bytes / 1024.0) + "KB";
    }
    else if (bytes < (1024 * 1024 * 1024))
    {
        return String(bytes / 1024.0 / 1024.0) + "MB";
    }
    else
    {
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
String getContentType(String filename, AsyncWebServerRequest *request)
{
    if (request->hasArg("download"))
    {
        return "application/octet-stream";
    }
    else if (filename.endsWith(".htm") || filename.endsWith(".html"))
    {
        return "text/html";
    }
    else if (filename.endsWith(".css"))
    {
        return "text/css";
    }
    else if (filename.endsWith(".js"))
    {
        return "application/javascript";
    }
    else if (filename.endsWith(".png"))
    {
        return "image/png";
    }
    else if (filename.endsWith(".gif"))
    {
        return "image/gif";
    }
    else if (filename.endsWith(".jpg"))
    {
        return "image/jpeg";
    }
    else if (filename.endsWith(".ico"))
    {
        return "image/x-icon";
    }
    else if (filename.endsWith(".xml"))
    {
        return "text/xml";
    }
    else if (filename.endsWith(".pdf"))
    {
        return "application/x-pdf";
    }
    else if (filename.endsWith(".zip"))
    {
        return "application/x-zip";
    }
    else if (filename.endsWith(".gz"))
    {
        return "application/x-gzip";
    }

    return "text/plain";
}

/**
 * @brief Handles requests for listing files in a specified directory.
 *
 * @param request The web server request.
 */
void handleFileList(AsyncWebServerRequest *request)
{
    if (!request->hasArg("dir"))
    {
        request->send(500, "text/plain", "BAD ARGS");
        return;
    }

    String path = request->arg("dir");
    Serial.println("handleFileList: " + path);

    File root = SPIFFS.open(path);
    path = String();

    String output = "[";

    if (root.isDirectory())
    {
        File file = root.openNextFile();

        while (file)
        {
            if (output != "[")
            {
                output += ',';
            }

            output += "{\"type\":\"";
            output += (file.isDirectory()) ? "dir" : "file";
            output += "\",\"path\":\"";
            output += String(file.path());
            output += "\",\"size\":\"";
            output += formatBytes(file.size());
            output += "\"}";
            file = root.openNextFile();
        }
    }
    root.close();
    output += "]";

    Serial.println("handleFileList: " + output);

    request->send(200, "text/json", output);
}

/**
 * @brief Restarts the ESP device upon receiving a request.
 *
 * @param request The web server request.
 */
void handleRestart(AsyncWebServerRequest *request)
{
    request->send(200);
    ESP.restart();
}

/**
 * @brief Redirects client requests to the main index page.
 *
 * @param request The web server request.
 */
void handleForwardIndex(AsyncWebServerRequest *request)
{
    IPAddress clientIP = request->client()->getLocalAddress();
    Serial.println("Webserver:" + clientIP.toString());
    if (WiFi.softAPIP() == clientIP)
    {
        request->redirect("http://"+apIP.toString()+"/wifi.html");
    }
    else
    {
        request->redirect("/index.html");
    }
}

/**
 * @brief Sends an HTTP 200 OK response.
 *
 * @param request The web server request.
 */
void handleOK(AsyncWebServerRequest *request)
{
    request->send(200);
}

/**
 * @brief Deletes a specified file on the filesystem.
 *
 * @param request The web server request.
 */
void handleFileDelete(AsyncWebServerRequest *request)
{
    String filename = "";
    if (request->hasParam("filename"))
    {
        filename = request->getParam("filename")->value();
    }

    if (filename == "")
    {
        request->send(400, "text/plain", "Filename missing");
        return;
    }

    if (SPIFFS.exists(filename))
    {
        if (SPIFFS.remove(filename))
        {
            Serial.println("File deleted: " + filename);
            request->send(200, "text/plain", "File deleted: " + filename);
        }
        else
        {
            Serial.println("Failed to delete file: " + filename);
            request->send(500, "text/plain", "Error deleting file");
        }
    }
    else
    {
        Serial.println("File not found: " + filename);
        request->send(404, "text/plain", "File not found: " + filename);
    }
    request->redirect("/");
}

/**
 * @brief Initializes the filesystem, with formatting if necessary.
 */
void initFS()
{
    if (!FileFS.begin(true))
    {
        Serial.println(F("Filesystem init failed! Formatting..."));

        if (!FileFS.begin())
        {
            while (true)
            {
                Serial.println(F("Failed to initialize filesystem."));
                delay(5000);
            }
        }
    }
}

/**
 * @brief Manages file upload through web server requests.
 */
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index)
    {
        request->_tempFile = SPIFFS.open("/" + filename, "w");
    }

    if (len)
    {
        request->_tempFile.write(data, len);
    }

    if (final)
    {
        request->_tempFile.close();
        request->redirect("/");
    }
}

void handleDownload(AsyncWebServerRequest *request)
{
    String filename = request->getParam("filename")->value();
    request->send(SPIFFS, filename);
}

void handleGetMem(AsyncWebServerRequest *request)
{
   String json = "{";
        json += "\"heapsize\":" + String(ESP.getHeapSize());
        json += ",\"heapfree\":" + String(ESP.getFreeHeap());
        json += ",\"heapmin\":" + String(ESP.getMinFreeHeap());
        json += ",\"heapmax\":" + String(ESP.getMaxAllocHeap());
        json += ",\"sketchsize\":" + String(ESP.getSketchSize());
        json += ",\"flashsize\":" + String(ESP.getFlashChipSize());
        json += ",\"spiffstotal\":" + String(SPIFFS.totalBytes());
        json += ",\"spiffsunused\":" + String(SPIFFS.usedBytes());
        json += "}";
    

    request->send(200, "text/json", json);
}

String wifiList()
{
    String json = "{\"wifilist\":[";
    int n = WiFi.scanNetworks(); // WLAN-Netzwerke scannen
    for (int i = 0; i < n; ++i)
    {
        if (i)
            json += ",";
        json += "{";
        json += "\"rssi\":" + String(WiFi.RSSI(i));
        json += ",\"ssid\":\"" + WiFi.SSID(i) + "\"";
        json += ",\"bssid\":\"" + WiFi.BSSIDstr(i) + "\"";
        json += ",\"channel\":" + String(WiFi.channel(i));
        json += ",\"secure\":" + String(WiFi.encryptionType(i));
        json += "}";
    }
    json += "]}";
    WiFi.scanDelete();
    return json;
}


void setWifi(AsyncWebServerRequest *request)
{
    if (request->hasParam("wifi", true))
    {
        String ssid = request->getParam("wifi", true)->value();
        String password = request->hasParam("password", true) ? request->getParam("password", true)->value() : "";

        Serial.printf("Versuche mit SSID: %s, Passwort: %s zu verbinden...\n", ssid.c_str(), password.c_str());

        WiFi.begin(ssid.c_str(), password.c_str()); // Mit dem angegebenen Netzwerk verbinden
        request->send(200, "text/plain", "Attempting to connect. Please wait...");
        preferences.putString("ssid", ssid);         // Save SSID
        preferences.putString("password", password); // Save Password

    }
    else
    {
        request->send(400, "text/plain", "Fehlende Parameter!");
    }
};

/**
 * @brief Initializes the web server with defined routes and handlers.
 */
void initWebserver()
{
    server.on("/list", HTTP_GET, handleFileList);
    server.on("/upload", HTTP_POST, handleOK, handleUpload);
    server.on("/delete-file", HTTP_GET, handleFileDelete);
    server.on("/reboot", HTTP_GET, handleRestart);
    server.on("/setWifi", HTTP_POST, setWifi);
    server.on("/getmem", HTTP_GET, handleGetMem);
    server.on("/", HTTP_GET, handleForwardIndex);
    server.on("/index", HTTP_GET, handleForwardIndex);
    server.on("/download", HTTP_GET, handleDownload);
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
void notifyClients(String sensorReadings)
{
    Serial.println(sensorReadings);
    ws.textAll(sensorReadings);
}

/**
 * @brief Called when specific data changes; sends updates to WebSocket clients.
 *
 * @param dataChange Enum value indicating the type of data change.
 */
void dadaChanged(change dataChange)
{
    switch (dataChange)
    {
    case ch_inputintern:
        notifyClients(jsonInputIntern());
        break;
    case ch_relay:
        notifyClients(jsonRelay());
        break;
    case ch_tempholdingreg:
        notifyClients(jsonTempHoldingReg());
        break;
    }
}

void sendWifiList()
{
    if (!WiFiScanTaskRunning)
    {
        WiFiScanTaskRunning = true;
        xTaskCreate([](void *) { // vTaskDelay(1000 / portTICK_PERIOD_MS);
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
void handleWebSocketMessage(void *arg, uint8_t *data, size_t len)
{
    AwsFrameInfo *info = (AwsFrameInfo *)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT)
    {
        data[len] = 0;
        String message = (char *)data;
        Serial.println("WS message: " + message);
        if (strchr((char *)data, '{') == NULL)
        {
            if (strcmp((char *)data, "getReadings") == 0)
            {
                dadaChanged(ch_inputintern);
                dadaChanged(ch_relay);
                dadaChanged(ch_tempholdingreg);
            }
            else if (strcmp((char *)data, "reloadPoints") == 0)
            {
                mypoints.build();
            }
            else if (strcmp((char *)data, "reboot") == 0)
            {
                ESP.restart();
            }
            else if (strcmp((char *)data, "wifilist") == 0)
            {
                sendWifiList();
            }
        }
        else
        {
            JsonDocument doc;
            deserializeJson(doc, (char *)data);
            JsonObject root = doc.as<JsonObject>();
            for (JsonPair kv : root)
            {
                String key = String(kv.key().c_str());
                if (key.startsWith("relays"))
                {
                    int pos = key.substring(6).toInt();
                    if (pos >= 0 && pos < RELAYS)
                    {
                        relays[pos] = kv.value().as<bool>();
                    }
                }
            }
        }
    }
}

/**
 * @brief Manages WebSocket events like connection, disconnection, and data reception.
 */
void onEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len)
{
    switch (type)
    {
    case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
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
void initWebSocket()
{
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

String getDynamicPassword()
{
    uint64_t chipId = ESP.getEfuseMac(); // Seriennummer (MAC-Adresse)
    String lastSix = String((uint32_t)chipId, HEX).substring(2); 
    return "config_" + lastSix;
}

void startAccessPoint()
{
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

void stopAccessPoint()
{
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
void WEBsetup()
{
    setDATAchanged(dadaChanged);

    mysetup->resetSection();
    mysetup->setNextSection("lan");
    WT32_ETH01_onEvent();
    String lanHostname = mysetup->getSectionValue<String>("hostname", "");
    if (!lanHostname.isEmpty()) {
        ETH.setHostname(mysetup->cstrPersists(lanHostname));
    }
    if (mysetup->getSectionValue<bool>("active", false)) {
        Serial.print("\nStarting WebServer on " + String(ARDUINO_BOARD));
        Serial.println(WEBSERVER_WT32_ETH01_VERSION);

        ETH.begin(
            ETH_PHY_ADDR, 
            mysetup->getSectionValue<int>("phy_power", ETH_PHY_POWER),
            mysetup->getSectionValue<int>("phy_mdc", ETH_PHY_MDC),
            mysetup->getSectionValue<int>("phy_mdio", ETH_PHY_MDIO),
            mysetup->getSectionValue<eth_phy_type_t>("type", ETH_PHY_TYPE),
            mysetup->getSectionValue<eth_clock_mode_t>("clk_mode", ETH_CLK_MODE)
        );
    }
    initWebSocket();
    initWebserver();

    mysetup->resetSection();
    mysetup->setNextSection("wifi");
    WiFiActive=mysetup->getSectionValue<bool>("active", WiFiActive);
    WiFiAptime=mysetup->getSectionValue<uint16_t>("aptime", WiFiAptime/1000)*1000;
    String wifiHostname = mysetup->getSectionValue<String>("hostname", "");
    if (!wifiHostname.isEmpty()) {
        ETH.setHostname(mysetup->cstrPersists(wifiHostname));
    }
    if (WiFiActive) {
        preferences.begin("wifi", false);

        // Versuche, mit gespeicherten Zugangsdaten zu verbinden
        String savedSSID = preferences.getString("ssid", "");
        String savedPassword = preferences.getString("password", "");

        if (savedSSID != "" && savedPassword != "")
        {
            Serial.println("Trying to connect with saved network...");
            WiFi.mode(WIFI_STA);
            WiFi.begin(savedSSID.c_str(), savedPassword.c_str());

            unsigned long startAttemptTime = millis();
            while (WiFi.status() != WL_CONNECTED)
            {
                if (millis() - startAttemptTime > 10000)
                { // 10 Sekunden Timeout
                    break;
                }
                delay(100);
            }
            if (WiFi.status() == WL_CONNECTED)
            {
                Serial.println("Successfully connected to saved network!");
                Serial.print("IP Address: ");
                Serial.println(WiFi.localIP());
            }
            else
            {
                Serial.println("Failed to connect to saved network.");
            }
        }
        if (WiFi.status() != WL_CONNECTED && WiFiAptime>0)
        {
            Serial.println("Starting Access Point...");
            String ssid = getDynamicSSID();
            String password = getDynamicPassword();
            startAccessPoint();
        }
    }
}

/**
 * @brief Manages WebSocket clients, cleaning up inactive clients.
 */
void WEBloop()
{
    ws.cleanupClients();
    if (accesspointActive) {
        dnsServer.processNextRequest();
        if (WiFiApstarttime == 0) WiFiApstarttime=millis();
        if (millis()-WiFiApstarttime > (WiFiAptime*1000)) {
            Serial.println("Ap wegen Zeitablauf gestoppt:"+String(WiFiAptime)+" < "+String(millis()-WiFiApstarttime));
            stopAccessPoint();
        }
    }
}
