#ifndef MYSERVER_H_
#define MYSERVER_H_

// Include necessary libraries for the KC868-A8S device and data handling.
#include "KC868-A8S.h"
#include "mySetup.h"

// Define the heartbeat interval in milliseconds. This is used to periodically
// print a heartbeat message.
#define HEARTBEAT_INTERVAL 10000L

// Include file system and SPIFFS library to interact with the file system.
#include <FS.h>
#include <SPIFFS.h>

// Set up the filesystem to use SPIFFS for file storage.
#define FileFS SPIFFS

/**
 * @brief Initializes the web server and related components.
 * This function sets up the web server, web socket, and Ethernet configuration.
 */
void WEBsetup();

/**
 * @brief Main loop for handling web server operations.
 * This function is called in the main loop to manage ongoing web server
 * interactions.
 */
void WEBloop();

/**
 * @brief Sends sensor readings to all connected clients.
 *
 * @param sensorReadings A string containing the sensor readings in JSON format.
 * This function broadcasts the provided sensor readings to all WebSocket
 * clients.
 */
void notifyClients(String sensorReadings);

#endif /* MYSERVER_H_ */
