#include "sha1.h"
#include "Base64.h"
#include <SoftwareSerial.h>
SoftwareSerial debug(8, 9); // RX, TX

void setup() {
  Serial.begin(9600);
  debug.begin(9600);
  delay(50);
  debug.println("start");
  Serial.println("AT+RST");
  delay(5000);
  Serial.println("AT+CWMODE=1");
  delay(1500);
  Serial.println("AT+CWJAP=\"SSID\",\"PASS\"\r\n");
  Serial.find("OK");
  Serial.println("AT+CIPMUX=1");
  Serial.find("OK");
  delay(3000);
  Serial.println("AT+CIPSERVER=1,8000");
  boolean server = Serial.find("OK");
  delay(3000);
  Serial.println("AT+CIFSR");
  boolean r = readLines(4);
  debug.println("einde setup");
  debug.println(server);
  boolean found = false;
  while(!found)
    found = Serial.find("Link");
  debug.println("linkje");
  debug.println("setup");
}


void loop() {
  String key = "";
  boolean isKey = Serial.find("Key: ");
  if(isKey) {
    debug.println("Key found!!");
      while(true) {
        if(Serial.available()) {
          char c = (char)Serial.read();
          if(c == '=') {
            doHandshake(key + "==");
            key = "";
            break;
          } 
          if(c != '\r' || c != '\n') {
            key = key + c;
          }
        }
      }
      while(true) {
          if(Serial.available()) {
            byte by = Serial.read();
            debug.print((char)by);
            debug.print(" ");
            debug.print((uint8_t)by);
            debug.print(" ");
            if(by & 0x80) {
              debug.println("Joehooeooeoe");
            }
          }
      }
      
  }
}

boolean readLines(int lines) {
  boolean found = false;
  int count = 0;
  while(count < lines) {
    if(Serial.available()) {
      char c = (char)Serial.read();
      if(c != '\r') {
        debug.write(c);
      } else {
        count++;
      } 
    }
  }
  return true;
}

bool doHandshake(String k) {
    debug.println("do handshake: " + k);
    char bite;
    char temp[128];
    char key[80];
    memset(temp, '\0', sizeof(temp));
    memset(key, '\0', sizeof(key));
    
    byte counter = 0;
    //x3JJHMbDL1EzLkh9GBhXDw==
    int myCo = 0;
    while ((bite = k.charAt(myCo++)) != 0) {
      key[counter++] = bite;
    }
    strcat(key, "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"); // Add the omni-valid GUID
    Sha1.init();
    Sha1.print(key);
    uint8_t *hash = Sha1.result();
    base64_encode(temp, (char*)hash, 20);
    debug.print(temp);
    
    int cc = -1;
    while(temp[cc++] != '\0') {} // cc is length return key
    //Serial.print("Lengte ");
    //Serial.println(cc);
    cc = 165 + cc; // length return key + 165 keys for rest of header
    
    Serial.print("AT+CIPSEND=0,");
    Serial.println(129);
    boolean found = false;
    while(!found)
      found = Serial.find(">");
    
    Serial.print("HTTP/1.1 101 Switching Protocols\r\n");
    Serial.print("Upgrade: websocket\r\n");
    Serial.print("Connection: Upgrade\r\n");
    Serial.print("Sec-WebSocket-Accept: ");
    Serial.print(temp);
    Serial.print("\r\n\r\n");
    return true;
}
