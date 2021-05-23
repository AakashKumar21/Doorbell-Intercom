#include <Arduino.h>
#include <ESP8266WiFi.h>
#define DEBUG_WEBSOCKETS(...) Serial.printf( __VA_ARGS__ )
#include <WebSocketsServer.h>
#include <WebSocketsClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiUdp.h>
#include <Ticker.h>
#include <tuple>
#include <iostream>
#include <array>
#include <utility>
#include "helper.h"

WiFiUDP Udp;
Ticker ticker;
StatusFlag currentFlag = NoFlag;

void statusChecker();
ICACHE_RAM_ATTR void ring_isr();

void setup() {
    pinMode(RING_PIN, INPUT_PULLUP);
    pinMode(ledPin, OUTPUT);
    pinMode(SRING_PIN, OUTPUT);
    pinMode(LRING_PIN, OUTPUT);
    attachInterrupt(digitalPinToInterrupt(RING_PIN), ring_isr, FALLING);
    startupBlink();
    Serial.begin(baudRate);
    Serial.setTimeout(2);
    WiFi.setOutputPower(20.5);
    #ifdef IS_AP
        createAP();
    #else
        connectToAP();
    #endif
    startUDP();
    ticker.attach_ms(500,statusChecker);
}

unsigned long PktsRcv = 0;

void loop() {
    // SERIAL DATA AVAILABLE
    if(Serial.available()){
        Serial.readBytes(udpBuffer,pktBuffSize);
        LOG("Sending UART Pkts.. ")
        #ifdef IS_AP
            Udp.beginPacket(BroadcastIP, ClientUdpPort);
        #else
            Udp.beginPacket(BroadcastIP, ServerUdpPort);
        #endif
        Udp.write(udpBuffer, pktBuffSize);
        Udp.endPacket();
        LOG("Sent!\n")
    }

    // UDP DATA AVAILABLE
    int packetSize = Udp.parsePacket();
    if (packetSize) // Data available
    {
        LOG("udp->")
        Udp.read(serBuffer, pktBuffSize);
        s_gotPing = true;

        switch(serBuffer[0]){
            // Type Audio
            case UdpAudioType:  LOG("audio->")
                                Serial.write(serBuffer, pktBuffSize); 
                                LOG('\n')
                                break;
            // Type any Ring
            case UdpSRingType:  sendRingSignal(UdpPacketType::UdpSRingType);
                                LOG("s.ring\n")
                                break;

            // Type long Ring 
            case UdpLRingType:  sendRingSignal(UdpPacketType::UdpLRingType);
                                LOG("l.ring\n")
                                break;
        }
    }
}

void startUDP(){
    #ifdef IS_AP
        auto udp_port = ServerUdpPort;
    #else
        auto udp_port = ClientUdpPort;
    #endif
    LOG("Starting UDP Server ") 
    while(!Udp.begin(udp_port)){
        LOG(".");
    }
    LOG(" Started at port:") LOG(udp_port) LOG('\n')
}

void statusChecker(){
    LOG(s_gotPing? "ping-":"");
    digitalWrite(ledPin, s_gotPing);
    s_gotPing = false;
    // PING
    #ifdef IS_AP
        Udp.beginPacket(BroadcastIP, ClientUdpPort);
    #else
        Udp.beginPacket(BroadcastIP, ServerUdpPort);
    #endif
    Udp.write(PING_BYTE);
    Udp.endPacket();
    LOG("pong\n")
    PktsRcv = 0;
}

void speedTest(){
    Serial.println("Starting Test");
    while(1)
    {
        #ifdef IS_AP
            Udp.beginPacket(BroadcastIP, ClientUdpPort);
        #else
            Udp.beginPacket(BroadcastIP, ServerUdpPort);
        #endif
        Udp.write(PING_BYTE);
        Udp.endPacket();
    }
    
}

ICACHE_RAM_ATTR void ring_isr(){
    static unsigned int prevTimestamp;
    // DEBOUNE
    // bool currentPinState = digitalRead(RING_PIN);
    // delay(40);
    // if(currentPinState != digitalRead(RING_PIN)) return;

    if((millis() - prevTimestamp) > 1000){
        #ifdef IS_AP
            Udp.beginPacket(BroadcastIP, ClientUdpPort);
        #else
            Udp.beginPacket(BroadcastIP, ServerUdpPort);
        #endif
        Udp.write(UdpPacketType::UdpSRingType);
        Udp.endPacket();
        prevTimestamp = millis();
        LOG("Sent Ring Signal\n")
    }
    else{
        LOG("Ring Cooldown, timeLeft: ") LOG(millis() - prevTimestamp) LOG('\n')
    }
}