#include <Arduino.h>
#include <ESP8266WiFi.h>
extern "C" {
  #include "user_interface.h"
}

String ssids[] = {"Test1", "Free Internet", "Just kidding", "BOEHKOMAT))0", "BA8A Z1N@ (2.4GHz)", "Prank", "Scam, don't click", "McDonald's Free WiFi", "Vkusno i Tochka Free WiFi))", "Shaurma u ashota free wifi"};
byte macs[42][6];
int count = 10;

int it = 0;
int ssidpointer = 59;

//Beacon Packet buffer
byte packet[128] = { 
  /* IEEE 802.11 Beacon Frame */
  /*0*/   0x80, 0x00, 0x00, 0x00,  //type/subtype = beacon
  /*4*/   0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, //dst addr = broadcast
  /*10*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //src addr = random MAC
  /*16*/  0x01, 0x02, 0x03, 0x04, 0x05, 0x06, //bss id = src addr
  /*22*/  0x90, 0x53, //seq number 1337 + frag number 0
  /*24*/  0xDE, 0xAD, 0x00, 0xBE, 0xEF, 0x00, 0xF0, 0x0D, //timestamp
  /*32*/  0x64, 0x00, //beacon interval (~100ms)
  /*34*/  0x01, 0x04, //capabilities info
  
  /* IEEE 802.11 wireless LAN managment frame */
  /*36*/  0x01, 0x08, //tag = Supported Rates, size
          0x82, 0x84, 0x8b, 0x96, 0x24, 0x30, 0x48, 0x6c, 
  /*46*/  0x03, 0x01, 0xFF, //tag = Channel, size, random channel
  
 /*49*/ 0x07, 0x06,  //tag = Conutry code
 /*51*/0x44,0x45, // DE
 /*53*/0x20, // enviroment any (32)
 /*54*/0x01, 0x0b,0x1b, //Country Info: First Channel Number: 1, Number of Channels: 11, Maximum Transmit Power Level: 27 dBm
  /*57*/  0x00, 0xFF, 0x23 //tag = SSID, size, prefix char
};

char countrycode[3] = "DE"; // Country code

void fillMACs() {
  for (int j = 0; j < count; j++) {
    macs[j][0] = random(256);
    macs[j][1] = random(256);
    macs[j][2] = random(256);
    macs[j][3] = random(256);
    macs[j][4] = random(256);
    macs[j][5] = random(256);
  }
}


void setup() {
  delay(100);
  wifi_set_opmode(STATION_MODE);
  wifi_promiscuous_enable(1);
  fillMACs();
  packet[51] = countrycode[0];
  packet[52] = countrycode[1];
}


void loop() {
  if (it == count) {
    it = 0;
  }

  byte channel = 1;
  wifi_set_channel(channel);
  packet[48] = channel;
  
  //Copying src addr and bssid
  packet[10] = packet[16] = macs[it][0];
  packet[11] = packet[17] = macs[it][1];
  packet[12] = packet[18] = macs[it][2];
  packet[13] = packet[19] = macs[it][3];
  packet[14] = packet[20] = macs[it][4];
  packet[15] = packet[21] = macs[it][5];

  //SSID
  byte currLenght = ssids[it].length();
  packet[ssidpointer-1] = currLenght;
  for (int i = ssidpointer; i < ssidpointer+currLenght; i++) {
    packet[i] = ssids[it][i-ssidpointer];
  }
  //Sending beacon 3 times
  for (int i = 0; i < 3; i++) {
    wifi_send_pkt_freedom(packet, ssidpointer+currLenght+8, 0);
    delay(1);
  }
  it++;
}
