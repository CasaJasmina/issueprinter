
#include<WiFiSSLClient.h>
#include <SPI.h>
#include <WiFi101.h>
#include "../include/ArduinoJson/DynamicJsonBuffer.hpp"
#include <ArduinoJson.h>

const char* ssid = "name-network";
const char* password = "password";

const char* host = "api.github.com";  //this address is given by Github itself
const int httpsPort = 443;


void setup() {

  DynamicJsonBuffer  jsonBuffer;    

  Serial.begin(115200);
  Serial.println();
  Serial.print("connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use WiFiClientSecure class to create TLS connection
  WiFiSSLClient client;
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }


  String url = "/orgs/casajasmina/events?page=1&per_page=2";
  Serial.print("requesting URL: ");
  Serial.println(url);

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n" +
               "Connection: close\r\n\r\n");

  Serial.println("request sent");
  while (client.connected()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") {
      Serial.println("headers received");
      break;
    }
  }
  String line = client.readStringUntil('\n');
  JsonArray& root = jsonBuffer.parseArray(line); 


  if (!root.success()) {
    Serial.println("parseArray() failed"); 
  }
  
  Serial.println("==========");
  Serial.print("Type of event: ");
  String type = root[0]["type"];
  Serial.println(type);
  
  Serial.print("Content: ");
  String body = root[0]["payload"]["issue"]["body"];
  Serial.println(body);
  Serial.println("==========");

  //  Serial.println("reply was:");  Uncomment This lines to print the String line, usefull for debugging
  //  Serial.println("==========");
  //  Serial.println(line);
  //  Serial.println("==========");
      Serial.println("closing connection");
}

void loop() {
}

