#include "myServer.h"
#include <ArduinoJson.h>

#define _ASYNC_WEBSERVER_LOGLEVEL_ 0

#include <AsyncTCP.h>
#include <WebServer_WT32_ETH01.h>
#include <WebServer_WT32_ETH01_Debug.h>
#include <ESPAsyncWebServer.h>
#include "myPoints.h"

/// Filesystem object, set to SPIFFS.
FS *filesystem = &SPIFFS;

/// Web server instance on port 80.
AsyncWebServer server(80);

/// WebSocket instance with URI "/ws".
AsyncWebSocket ws("/ws");

/// File handle for uploading files to the filesystem.
File fsUploadFile;

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
    request->redirect("/index.html");
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

/**
 * @brief Initializes the web server with defined routes and handlers.
 */
void initWebserver()
{
    server.on("/list", HTTP_GET, handleFileList);
    server.on("/upload", HTTP_POST, handleOK, handleUpload);
    server.on("/delete-file", HTTP_GET, handleFileDelete);
    server.on("/reboot", HTTP_GET, handleRestart);
    server.on("/", HTTP_GET, handleForwardIndex);
    server.on("/index", HTTP_GET, handleForwardIndex);
    server.on("/download", HTTP_GET, handleDownload);
    server.serveStatic("/", SPIFFS, "/www/");
    server.serveStatic("/config/", SPIFFS, "/config/");
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

/**
 * @brief Configures and initializes the web server, WebSocket, and Ethernet.
 */
void WEBsetup()
{
    Serial.print("\nStarting WebServer on " + String(ARDUINO_BOARD));
    Serial.println(" with " + String(SHIELD_TYPE));
    Serial.println(WEBSERVER_WT32_ETH01_VERSION);

    WT32_ETH01_onEvent();
    ETH.begin(ETH_PHY_ADDR, ETH_PHY_POWER, ETH_PHY_MDC, ETH_PHY_MDIO, ETH_PHY_TYPE, ETH_CLK_MODE);
    initWebSocket();
    initWebserver();
    setDATAchanged(dadaChanged);
}

/**
 * @brief Manages WebSocket clients, cleaning up inactive clients.
 */
void WEBloop()
{
    ws.cleanupClients();
}
