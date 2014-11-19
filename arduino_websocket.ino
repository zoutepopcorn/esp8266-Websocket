#include "sha1.h"
#include "Base64.h"
#include <SoftwareSerial.h>
#include <MemoryFree.h>
#define MAX_RECEIVE_MESSAGE_SIZE 256
/*
  This is a websocket test for the ESP8266 with some code from:
  - https://github.com/ejeklint/ArduinoWebsocketServer
  - https://github.com/krohling/ArduinoWebsocketClient/blob/master/WebSocketClient.cpp
  
  USAGE
  Fill in your SSID and PASSWORD from the router in the code
  
  Simple test you can do on: http://www.websocket.org/echo.html (use http NOT https)
  Fill in ws://ip.from.the.esp:8000 and then click connect
  Then you can send a sort message from the website, by example "123456"
  
  The other way arround is not working.. :(. 
  
  Chrome gives the error:
  
  Chrome error:

  WebSocket connection to 'ws://192.168.1.101:8000/?encoding=text' failed: 
  One or more reserved bits are on: reserved1 = 0, reserved2 = 1, reserved3 = 1 
  
*/ 
// 0x02  Binary frame
// 0x08  Close connection
// 0x09  Ping frame
// 0x0A  Pong frame
// 0x0F  Bit mask for opcodes

SoftwareSerial debug(8, 9); // RX, TX
boolean isLink = false;

struct Frame {
    bool isMasked;
    bool isFinal;
    byte opcode;
    byte mask[4];
    byte length;
    char data[MAX_RECEIVE_MESSAGE_SIZE];
} frame;

void setup() {
  Serial.begin(38400);
  debug.begin(38400);
  delay(50);
  debug.println("start");
  Serial.println("AT+RST");
  delay(5000);
  Serial.println("AT+CWMODE=1");
  delay(1500);
   // Fill in your acces point
  Serial.println("AT+CWJAP=\"YOUR ROUTER SSID\",\"PASS\""); 
  delay(1500);
  Serial.println("AT+CIPMUX=1");
  Serial.find("OK");
  delay(3000);
  Serial.println("AT+CIPSERVER=1,8000");
  boolean server = Serial.find("OK");
  delay(3000);
  Serial.println("AT+CIFSR");   // Display the ip please
  boolean r = readLines(4);
  debug.println("eind setup");
  debug.println(server);
  boolean found = false;
  
  debug.println("link builded, end setup");
}


void loop() {
  
  
  
  if(isLink) {
      String key = "";
      boolean isKey = Serial.find("Key: ");
      if(isKey) {
        debug.println("Key found!");
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
        // _________________________ PROBLEMO ____________________________________
        while(true) {  // So far so good. Handshake done Now wait for the message
           Serial.println("AT+CIPSEND=0,6");
           boolean found = Serial.find(">");
           if(found) {
             sendMessage("test", 4); // Error by sending
             isLink = false;
             break;
           }
        }
        // _________________________ /PROBLEMO ____________________________________
      }
  } else {
     isLink = Serial.find("Link");
  }

}


boolean getFrame() {
    debug.println("getFrame()");
    byte bite;
    unsigned short payloadLength = 0;
    
    bite = Serial.read();        
    frame.opcode = bite & 0xf; // Opcode
    frame.isFinal = bite & 0x80; // Final frame?
    bite = Serial.read();
    frame.length = bite & 0x7f; // Length of payload        
    frame.isMasked = bite & 0x80;

    // Frame complete!
    if (!frame.isFinal) {
        return false;
    }
    // First check if the frame size is within our limits.
    if (frame.length > 126) {       
        return false;
    }

    // If the length part of the header is 126, it means it contains an extended length field.
    // Next two bytes contain the actual payload size, so we need to get the "true" length.
     if (frame.length == 126) {
        byte exLengthByte1 = Serial.read();
        byte exLengthByte2 = Serial.read();
        payloadLength = (exLengthByte1 << 8) + exLengthByte2;
     } 
     // If frame length is less than 126, that is the size of the payload.
     else {
        payloadLength = frame.length;        
     }

     // Check if our buffer can store the payload.
     if (payloadLength > MAX_RECEIVE_MESSAGE_SIZE) {
        debug.println("te groot");
        return false;
     }

    // Client should always send mask, but check just to be sure    
    if (frame.isMasked) {
        frame.mask[0] = Serial.read();
        frame.mask[1] = Serial.read();
        frame.mask[2] = Serial.read();
        frame.mask[3] = Serial.read();
    }
    
    // Get message bytes and unmask them if necessary
    for (int i = 0; i < payloadLength; i++) {
        if (frame.isMasked) {
            frame.data[i] = Serial.read() ^ frame.mask[i % 4];
        } else {
            frame.data[i] = Serial.read();
        }
    }
    
    for (int i = 0; i < payloadLength; i++) {
        debug.print(frame.data[i]);
        if(frame.data[i] == '/r')
          break;
    } 
    return true;
}

boolean sendMessage(char *data, byte length) {    
 
      Serial.print((uint8_t) 0b00000000);   // 
      //Serial.print((uint8_t) 0x1); // Txt frame opcode
      
      Serial.print((uint8_t) length); // Length of data
      //Serial.print((uint8_t) 0x00);
      //Serial.print((uint8_t) 0x00);
      //Serial.print((uint8_t) 0x00);
      //Serial.print((uint8_t) 0x00);
      
      for (int i = 0; i < length ; i++) {
          Serial.print(data[i]);
      }
      delay(1);
      return true;
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
    cc = 165 + cc; // length return key + 165 keys for rest of header
    
    Serial.print("AT+CIPSEND=0,");
    Serial.println(129);  // +30   // was 129
    boolean found = false;
    while(!found)
      found = Serial.find(">");  // Wait until I can send
    Serial.print("HTTP/1.1 101 Switching Protocols\r\n");
    Serial.print("Upgrade: websocket\r\n");
    Serial.print("Connection: Upgrade\r\n");
    Serial.print("Sec-WebSocket-Accept: ");
    Serial.print(temp);
    Serial.print("\r\n\r\n");
    return true;
}
