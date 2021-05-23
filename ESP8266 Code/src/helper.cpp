#include "helper.h"


bool createAP(){
    // String apName = WifiName;
    // apName += ESP.getChipId();
    LOG("Creating AP: ") LOG(WifiName);
    WiFi.mode(WIFI_AP);
    bool isCreated = WiFi.softAP(WifiName, WifiPass);
    IPAddress myIP = WiFi.softAPIP();
    LOG("AP IP address: ") LOG(myIP) LOG('\n')
    LOG(isCreated? "Created":"Failed Creating AP\n")
    return isCreated;
}

bool connectToAP(){
    LOG("Connecting to: ") LOG(WifiName) LOG(" ");
    WiFi.mode(WIFI_STA);
    WiFi.enableSTA(true);
    WiFi.begin(WifiName, WifiPass);
    auto startTime = millis();
    while(millis() < startTime + connectTimeMax){
        LOG('.')
        if(WiFi.status() == WL_CONNECTED){
            LOG('\n') 
            LOG("[Wifi] Connected, IP: ") LOG(WiFi.localIP())
            return 1;
        }
        delay(500);
    }
    LOG("Can't Connect");
    ESP.restart();
    return 0;
}

void startupBlink(){
    #ifdef IS_AP
        int blinkDelay = 500;
    #else
        int blinkDelay = 200;
    #endif
    for(int i=0; i<2; i++){
        digitalWrite(ledPin,LOW);
        delay(blinkDelay);
        digitalWrite(ledPin,HIGH);
        delay(blinkDelay);
    }
}

void sendRingSignal(int _ringType){
    if(_ringType == UdpPacketType::UdpSRingType){ 
        digitalWrite(SRING_PIN, HIGH);
        delayMicroseconds(100);
        digitalWrite(SRING_PIN, LOW);
    }
    if(_ringType == UdpPacketType::UdpLRingType){ 
        digitalWrite(LRING_PIN, HIGH);
        delayMicroseconds(100);
        digitalWrite(LRING_PIN, LOW);
    }
}