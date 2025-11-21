This is te firmware for the Home intrusion detector.
The Home intrusion detctor uses a PIR sensor to detect human presence, an esp32 microcontrller development board that connects to wifi.
The esp32 firsts connects o wifi.
Then the immediately the PIR detects human presence in the room, an interrupt is triggered.
In the ISR(interrupt serice routine), the esp32 send a messsage to yumidebot(a telegram bot) saying: "🔔*INTRUSION DETCTED*, GO CHECK IT OUT OR CALL THE POLICE NOW!
The Home owner can also turn the diviece on/off, simply by sending "on,off"

