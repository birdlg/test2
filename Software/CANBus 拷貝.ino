
#pragma GCC diagnostic ignored "-Wwrite-strings"

#include "html.h"
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h>
#include "SSD1306.h"

long period;
#define SSID "AWV"                                   // WiFi ESSID
#define PASS "connectme"                             // WiFi password
#define hostname "dash1"
// Instantiate server class                          // SSD1306 instance
SSD1306  display(0x3c, D1, D2);
ESP8266WebServer server(80);                         // webserver instance at port 80

String disp_msg;

void handle_msg()                                   // handler of /msg
{
  server.send(200, "text/html", HTML_INDEX);        // Send same page so they can send another msg
  String message = (server.arg("message"));
  Serial.println(message);
  disp_msg = message;
  Blink();
  String buzz = (server.arg("buzz"));
  Serial.println(buzz);
}

void handle_root()                                  // handler of /
{
    server.send(200, "text/html", HTML_INDEX);
}

void setup(void) 
{
  //ESP.wdtDisable();                                     // used to debug, disable wachdog timer,
  Serial.begin(115200);                                   // full speed to monitor (actually can be faster...)
   
  WiFi.begin(SSID, PASS);                                 // Connect to WiFi network

  while (WiFi.status() != WL_CONNECTED) {                 // Wait for connection
    delay(500);
    Serial.print(".");
  }
  
  Serial.println();
  Serial.print("SSID : ");                                // prints SSID in monitor
  Serial.println(SSID);                                   // to monitor
  Serial.println(WiFi.localIP());                         // Serial monitor prints localIP
  
  if (!MDNS.begin(hostname)) {
    Serial.println("Error setting up mDNS responder!");   // setuo mDNS responder
    while(1) { 
      delay(1000);
    }
  }

  // Set up the endpoints for HTTP server,  Endpoints can be written as inline functions:
  server.on("/", handle_root);                            // http handler for "/"
  server.on("/msg", handle_msg);                          // http handler for "/msg"
  server.begin();                                         // Start the server
  
  MDNS.addService("http", "tcp", 80);                     // register web server to mDNS
  Serial.println("Web server up.");

  display.init();
  display.flipScreenVertically();
  display.clear();
  ShowStatus();
  delay(1000);
}

void ShowStatus()
{
  display.drawRect(0, 0, 127, 63);
  display.setFont(ArialMT_Plain_10);
  display.drawString(10, 10, String("Connected to WiFi: \n") + SSID);
  display.display();
}
void Blink()
{
  int i;
  for(i=0;i<3;i++)
  {
    display.clear();
    display.fillRect(0,0,127,63);
    display.display();
    delay(100);
    display.clear();
    display.display();
    delay(100);
  } 
}

void update_display(int delay)
{
  display.clear();
  if(disp_msg.length() < 50)
    display.setFont(ArialMT_Plain_16);
  else
    display.setFont(ArialMT_Plain_10);
  display.drawStringMaxWidth(random(delay+2), random(delay+2), 118, disp_msg);
  display.display();
}

void loop(void) {
  static long last_time;
  static int delay=1000;
  server.handleClient();                                  // checks for incoming messages
  if(millis() > last_time + delay)
  {
    Serial.println("update");
    update_display(delay/100);
    last_time=millis();
    delay-=(delay/100);
    Serial.println(delay);
    if (delay<100) delay=1000;
  }
}
