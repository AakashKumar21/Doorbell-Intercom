#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

constexpr char ssid[] = "Intercom";
constexpr char pass[] = "inter1234";

bool ap_stn_num_change = true;
int  ap_stn_count = 0;

WiFiUDP Udp;
unsigned int localUdpPort = 4210;
char incomingPacket[512];
char  replyPacket[] = "ACK";

IPAddress local_IP(192,168,4,22);
IPAddress gateway(192,168,4,9);
IPAddress subnet(255,255,255,0);

void setup() {
  Serial.begin(115200);
  Serial.println();

  Serial.print("Setting soft-AP configuration ... ");
  Serial.println(WiFi.softAPConfig(local_IP, gateway, subnet) ? "Ready" : "Failed!");

  Serial.print("Setting soft-AP ... ");
  Serial.println(WiFi.softAP("ESPsoftAP_01") ? "Ready" : "Failed!");

  Serial.print("Soft-AP IP address = ");
  Serial.println(WiFi.softAPIP());
  Udp.begin(localUdpPort);
}

void loop()
{
  if(WiFi.softAPgetStationNum() != ap_stn_count){
    ap_stn_count = WiFi.softAPgetStationNum();
    Serial.print("Connected: "); Serial.println(ap_stn_count);
  }
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    // receive incoming UDP packets
    Serial.printf("Received %d bytes from %s, port %d\n", packetSize, Udp.remoteIP().toString().c_str(), Udp.remotePort());
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    Serial.printf("UDP packet contents: %s\n", incomingPacket);

    // send back a reply, to the IP address and port we got the packet from
    Udp.beginPacket(Udp.remoteIP(), Udp.remotePort());
    Udp.write(replyPacket);
    Udp.endPacket();
  }
}
