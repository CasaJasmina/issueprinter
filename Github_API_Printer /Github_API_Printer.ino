/*

 This code connects to an unencrypted Wifi network and
 print out the events of a certain repository from github.com.
 It reads github's api as a string and parse it using
 the Json library.

 Circuit:
  Arduino zero
  WiFi 101 shield attached

*/

#include<WiFiSSLClient.h>
#include <SPI.h>
#include <WiFi101.h>
#include "../include/ArduinoJson/DynamicJsonBuffer.hpp"
#include <ArduinoJson.h>

const char* ssid = "myNetwork";
const char* password = "myPassword";

const char* host = "api.github.com";  //this address is given by Github itself
const int httpsPort = 443;
WiFiSSLClient client;
int Old_id = 0;

void setup() {     ////////////////////////////////////////////////////////////////////////////////

  Serial.begin(9600);
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
  Serial.print("connecting to ");
  Serial.println(host);
  if (!client.connect(host, httpsPort)) {
    Serial.println("connection failed");
    return;
  }
}

                        // this method makes a HTTP connection to the server:
void httpRequest() {   ////////////////////////////////////////////////////////////////////////////////

  String url = "/orgs/casajasmina/events?page=1&per_page=1";  //   /orgs/myRepository/events
  Serial.print("requesting URL: ");                           //  " ?page=1&per_page=1 " add this to get events
  Serial.println(url);                                        //    one by one and not to fill SRAM memory

  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "User-Agent: BuildFailureDetectorESP8266\r\n\r\n" );
  Serial.println("request sent");
}



void loop() {    ////////////////////////////////////////////////////////////////////////////////

  DynamicJsonBuffer  jsonBuffer;

  httpRequest();

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



  int New_id = root[0]["id"];
  Serial.println(New_id);         //Compare id to not print old events

  if (Old_id != New_id) {

    Serial.println("==========");

    Serial.print("Type of event: ");
    String type = root[0]["type"];
    Serial.println(type);

    Serial.print("Repo : ");
    String repo = root[0]["repo"]["name"];
    Serial.println(repo);

    Serial.print("Title: ");
    String title = root[0]["payload"]["title"];
    Serial.println(title);

    Serial.print("Posted by : ");
    String login = root[0]["actor"]["login"];
    Serial.println(login);

    Serial.print("Content: ");
    String body = root[0]["payload"]["issue"]["body"];
    Serial.println(body);


    Serial.println("==========");

    //  Serial.println("reply was:");  //Uncomment these lines to print the whole string recieved from github
    //  Serial.println("==========");
    //  Serial.println(line);
    //  Serial.println("==========");
    Serial.println("closing connection");

    Old_id = New_id;
  }
  else {
    Serial.println("There are no new events");
  }

  delay(60*1000); //github allows you to make only 60 request per hour
}
