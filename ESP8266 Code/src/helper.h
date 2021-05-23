#ifndef HELPER_H
#define HELPER_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <WebSocketsServer.h>

enum StatusFlag{
    NoFlag,
    SRingFlag,
    LRingFlag
};

enum UdpPacketType{
    UdpAudioType,
    UdpSRingType,
    UdpLRingType
};

enum RingType{
    Short,
    Long
};

// #define LOG_ENABLED
#ifdef LOG_ENABLED
    #define LOG(arg) Serial.print(arg);
#else 
    #define LOG(arg)
#endif

constexpr auto baudRate     = 115200*8;
constexpr auto pktBuffSize  = 128;
constexpr auto ptkToSend    = 3200000;

constexpr auto ledPin       = 2;
constexpr auto RING_PIN     = 0;
constexpr auto LRING_PIN    = 14; // D5
constexpr auto SRING_PIN    = 12; // D6
constexpr auto benchTime    = 10000;

constexpr char PING_BYTE = 255;
constexpr char RING_BYTE = 233;

constexpr auto connectTimeMax = 10000;

static char udpBuffer[pktBuffSize] = {0}; // uart -> udp
static char serBuffer[pktBuffSize] = {0}; // udp  -> uart

static bool s_gotPing = false;

static IPAddress ServerIP(192,168,4,1);
static IPAddress ClientIP(192,168,4,2);
static IPAddress BroadcastIP(192,168,4,255);
static IPAddress wsServerIP(192,168,4,1);
constexpr auto wsPort       = 81;
static constexpr int ServerUdpPort = 2000;
static constexpr int ClientUdpPort = 2001;

constexpr char WifiName[] {"ESP-Link"};
constexpr char WifiPass[] {"GG789343"};

// constexpr char WifiName[] {"ESP-Link"};
// constexpr char WifiPass[] {"GG789343"};

bool createAP();
bool connectToAP();
// Connect if nearest wifi is found and return enum

// Handlers
void onDisconnectHandler(WStype_t type, uint8_t * payload, size_t length);
void onConnectHandler(WStype_t type, uint8_t * payload, size_t length);
void onErrorHandler(WStype_t type, uint8_t * payload, size_t length);

ICACHE_RAM_ATTR void isr_benchmark();
void startupBlink();

void startUDP();

void sendRingSignal(int _ringType);



#endif