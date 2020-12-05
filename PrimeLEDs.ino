#include <SPI.h>
#include <WiFiNINA.h>

#include "arduino_secrets.h" 

#define PIN_RED     5
#define PIN_GREEN   4
#define PIN_BLUE    3
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)
int keyIndex = 0;                 // your network key Index number (needed only for WEP)

int status = WL_IDLE_STATUS;

WiFiServer server(80);

// current state
int state = 0;

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);   // PIN 13 LED
  pinMode(PIN_RED, OUTPUT);
  pinMode(PIN_GREEN, OUTPUT);
  pinMode(PIN_BLUE, OUTPUT);

  digitalWrite(PIN_RED, LOW);
  digitalWrite(PIN_GREEN, LOW);
  digitalWrite(PIN_BLUE, LOW);

    Serial.begin(9600);                 // DEBUG

    // check for the WiFi module:
    if (WiFi.status() == WL_NO_MODULE) {
      Serial.println("Communication with WiFi module failed!");
      // don't continue
      while (true);
    }  

    String fv = WiFi.firmwareVersion();
    if (fv < WIFI_FIRMWARE_LATEST_VERSION) {
      Serial.println("Please upgrade the firmware");
    }
  
    // attempt to connect to Wifi network:
    while (status != WL_CONNECTED) {
      Serial.print("Attempting to connect to Network named: ");
      Serial.println(ssid);                   // print the network name (SSID);
  
      // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
      status = WiFi.begin(ssid, pass);
      // wait 10 seconds for connection:
      delay(10000);
    }
    server.begin();                           // start the web server on port 80
    printWifiStatus();                        // you're connected now, so print out the status     
}

void loop() {
    // For a set of NeoPixels the first NeoPixel is 0, second is 1, all the way up to the count of pixels minus one.
    Serial.println("Head of loop");     // DEBUG

    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {                             // if you get a client,
        Serial.println("new client");           // print a message out the serial port
        String currentLine = "";                // make a String to hold incoming data from the client
        while (client.connected()) {            // loop while the client's connected
            if (client.available()) {             // if there's bytes to read from the client,
                char c = client.read();             // read a byte, then
                Serial.write(c);                    // print it out the serial monitor
                if (c == '\n') {                    // if the byte is a newline character

                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0) {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        client.println("HTTP/1.1 200 OK");
                        client.println("Content-type:text/html");
                        client.println();

                        // the content of the HTTP response follows the header:
                        client.print("Click <a href=\"/white\">here</a> turn the LEDs white<br>");
                        client.print("Click <a href=\"/red\">here</a> turn the LEDs red<br>");
                        client.print("Click <a href=\"/blue\">here</a> turn the LEDs blue<br>");
                        client.print("Click <a href=\"/purple\">here</a> turn the LEDs purple<br>");
                        client.print("Click <a href=\"/off\">here</a> turn the LEDs off<br>");

                        // The HTTP response ends with another blank line:
                        client.println();
                        // break out of the while loop:
                        break;
                    } else {    // if you got a newline, then clear currentLine:
                        currentLine = "";
                    }
                } else if (c != '\r') {  // if you got anything else but a carriage return character,
                    currentLine += c;      // add it to the end of the currentLine
                }

                // Check to see if the client request was "GET /H" or "GET /L":
                if (currentLine.endsWith("GET /white")) {
                    state = 1;
                }
                if (currentLine.endsWith("GET /red")) {
                    state = 2;
                }            
                if (currentLine.endsWith("GET /blue")) {
                    state = 3;
                }
                if (currentLine.endsWith("GET /purple")) {
                    state = 4;
                }                                    
                if (currentLine.endsWith("GET /off")) {
                    state = 0;
                }
            }
        }

        // close the connection:
        client.stop();
        Serial.println("client disonnected");
    }
  
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(1000);  

  if (state == 0) {  
    digitalWrite(PIN_RED, LOW);   
    digitalWrite(PIN_GREEN, LOW);   
    digitalWrite(PIN_BLUE, LOW);   
  }   
  else if (state == 1) {  
    digitalWrite(PIN_RED, HIGH);   
    digitalWrite(PIN_GREEN, HIGH);   
    digitalWrite(PIN_BLUE, HIGH);   
  }     
  else if (state == 2) {  
    digitalWrite(PIN_RED, HIGH);   
    digitalWrite(PIN_GREEN, LOW);   
    digitalWrite(PIN_BLUE, LOW);   
  } 
  else if (state == 3) {  
    digitalWrite(PIN_RED, LOW);   
    digitalWrite(PIN_GREEN, LOW);   
    digitalWrite(PIN_BLUE, HIGH);   
  } 
  else if (state == 4) {  
    digitalWrite(PIN_RED, HIGH);   
    digitalWrite(PIN_GREEN, LOW);   
    digitalWrite(PIN_BLUE, HIGH);   
  }  

}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your board's IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.print(rssi);
  Serial.println(" dBm");
  // print where to go in a browser:
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}
