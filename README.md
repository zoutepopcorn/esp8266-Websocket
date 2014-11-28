esp8266-Websocket
=================

At this moment not working so much at this websocket. I try to implement it in the esp itself later.

Testing a Websocket... 

For now not working and @ arduino but I want to make it run on the ESP itself.

Sending a message is broken.

I used some code from this lib:
- https://github.com/ejeklint/ArduinoWebsocketServer
- https://github.com/krohling/ArduinoWebsocketClient/

TODO: 
- https://github.com/mc96107/Arduino-Websocket/blob/master/WebSocketClient.cpp
- debug with Wireshark..

My ESP8266 is @ 38400 (Arduino pro mini @ 3.3V Its the max speed)

Websocket handshake (working)
http://en.wikipedia.org/wiki/WebSocket

Testing websocket online:
http://www.websocket.org/echo.html

My goal is to make it also make it working @  ESP8266.

This is a nice topic, about websocket:
http://stackoverflow.com/questions/8125507/how-can-i-send-and-receive-websocket-messages-on-the-server-side

INSTRUCTIONS
Connect ESP8266 to 3.3 and CH_PD to +. Then RX & TX to the RX and TX from your aduino. Pin 9 to RX from your debug Serial port. Don't put it at 5 Volt!!

<img src="http://www.electrodragon.com/w/images/5/5f/ESP8266_V091.png" alt="Image">
