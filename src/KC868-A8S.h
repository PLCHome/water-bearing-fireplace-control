#ifndef KC868_A8S_H
#define KC868_A8S_H

// KC868-A8S Smart Controller Pinout...
// https://www.kincony.com

//ETH
#ifdef ETH01
#define ETH_PHY_TYPE  ETH_PHY_LAN8720
#define ETH_PHY_ADDR  0
#define ETH_PHY_MDC   23
#define ETH_PHY_MDIO  18
#define ETH_PHY_POWER -1
#define ETH_CLK_MODE  ETH_CLOCK_GPIO17_OUT
#endif

//modbus
#define RS485_DEFAULT_TX_PIN 33
#define RS485_DEFAULT_RX_PIN 32
#define RS485_DEFAULT_DE_PIN -1
#define MODBUS_BUAD 9600
#define MODBUS_CONFIG SERIAL_8N1
#define MODBUS_SERIAL Serial2

//i2c
#define I2C_SDA 4
#define I2C_SDL 5
#define PCF8574_I1_ADDR 0x22
#define PCF8574_R1_ADDR 0x24
#define PCF8574_INPUTS 8
#define PCF8574_OUTPUTS 8

//beep
#define BEEP_PIN 2

//DS18B20 input pin
#define DS18B20_PIN 14

//WS2812 input pin
#define WS2812_PIN 12
#define WS2812_LENGTH 1
#define WS2812_PIXEL 0

//Analog pin
#define ANALOG_A1_PIN   36
#define ANALOG_A2_PIN   39
#define ANALOG_A3_PIN   34
#define ANALOG_A4_PIN   35

//button Upload
#define USER_BTN_PIN 0

//GSM
#define GSM_RX_PIN 13
#define GSM_TX_PIN 15

//GSM
#define GSM_433_PIN 16

#endif
