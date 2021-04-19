
#include "EspMQTTClient.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 23 //Datenleitung wird über GPIO-Pin 23 Angeschlossen
OneWire oneWire(ONE_WIRE_BUS); //Einrichten von OneWire

DallasTemperature sensors(&oneWire);


EspMQTTClient client(
  "IDIDIDID", //Wlan Name
  "pwpwpwpw", //Wlan passwort
  "192.168.0.144",  // MQTT Broker server ip
  "MQTTUsername",   // Can be omitted if not needed
  "MQTTPassword",   // Can be omitted if not needed
  "Terrarium",     // Client name that uniquely identify your device
  1883              // The MQTT port, default to 1883. this line can be omitted
);

String ein = "true"; //String zum vergleichen für Binärwandlung
String aus = "false";//String zum vergleichen für Binärwandlung
String r1 = "false"; //Röhre 1 String
int r1state = LOW;// Status lesbar für digital Write

String r2 = "false";//String für Röhre 2
int r2state = LOW;

String h1 = "false";//String für Halogenlampe 1
int h1state = LOW;

String w1 = "false";//String für Wüstenlampe 1
int w1state = LOW;

long timetemp; //Zwischenspeicher für Zeitwert
long timetempdelay = 60000; //Wartezeit zum Übertragen der Temperatur
String temp1;//Zwischenspeicher Temperatur


void setup()

{

  pinMode(22, OUTPUT);//Röhre1
  pinMode(21, OUTPUT);//Röhre2
  pinMode(19, OUTPUT);//Halogenlampe 1
  pinMode(18, OUTPUT);//Halogenlampe 2

  
  
  Serial.begin(115200);

  // Optionnal functionnalities of EspMQTTClient : 
  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("TestClient/lastwill", "I am going offline");  // You can activate the retain flag by setting the third parameter to true
}

// This function is called once everything is connected (Wifi and MQTT)
// WARNING : YOU MUST IMPLEMENT IT IF YOU USE EspMQTTClient
void onConnectionEstablished()
{
  // Subscribe to "röhre1" and display received message to Serial
  client.subscribe("terrarium/r1", [](const String & payload) {
   
    //Serial.println(payload);
    r1 = payload;   
    // Serial.println(r1);
    if (r1 == ein)  //prüfen ob r1 = true
     {
               r1state = HIGH;// Dann Zwischenspeicher auf High
               //Serial.print("ein");
    }
    if (r1 == aus ) //prüfen ob r1 = false 
    {
              r1state = LOW; //Dann Zwischenspeicher auf Low
              //Serial.print("aus");
    }
 digitalWrite(22, r1state); //Setze Ausgang 22 => Röhre 1 auf den Wert de Zwischenspeichers
  });

  //Röhre 2
    client.subscribe("terrarium/r2", [](const String & payload) {
   
   // Serial.println(payload);
    r2 = payload;   
    // Serial.println(r2);
    if (r2 == ein)  
     {
               r2state = HIGH;
               //Serial.print("ein");
    }
    if (r2 == aus)
    {
              r2state = LOW;
              //Serial.print("aus");
    }
 digitalWrite(21, r2state);
  });


  //Halogenlampe
    client.subscribe("terrarium/h1", [](const String & payload) {
   
    //Serial.println(payload);
    h1 = payload;   
    // Serial.println(h1);
    if (h1 == ein)  
     {
               h1state = HIGH;
             //  Serial.print("ein");
    }
    if (h1 == aus)
    {
              h1state = LOW;
            //  Serial.print("aus");
    }
 digitalWrite(19, h1state);
  });

//Wüstenlampe
    client.subscribe("terrarium/w1", [](const String & payload) {
   
    //Serial.println(payload);
    w1 = payload;   
     //Serial.println(h1);
    if (w1 == ein)  
     {
               w1state = HIGH;
              // Serial.print("ein");
    }
    if (w1 == aus)
    {
              w1state = LOW;
              //Serial.print("aus");
    }
 digitalWrite(18, w1state);
  });

  //Überbleibsel aus Beispielskecht bei nächstem Update deaktivieren!
  // Subscribe to "mytopic/wildcardtest/#" and display received message to Serial


  // Publish a message to "mytopic/test"
  client.publish("mytopic/test", "This is a message"); // You can activate the retain flag by setting the third parameter to true

  // Execute delayed instructions
  client.executeDelayed(5 * 1000, []() {
    client.publish("mytopic/wildcardtest/test123", "This is a message sent 5 seconds later");
  });
}

void loop()
{
  client.loop();// ausführen der Abfragen


if (millis() > timetemp + timetempdelay) {//Zeitverzögerung Tempauslesen nur einmal alle x s
  timetemp = millis ();
  sensors.requestTemperatures(); // Sendet das Kommando , um die Temperaturdaten zu erhalten 
  float value = sensors.getTempCByIndex(0);//Schreibe die Tempdaten nach value
  temp1 = String(value,2);//Wandle value in einen String mit 2 Dezimalstellen
  Serial.println(value);
  client.publish("terrarium/temp1", temp1);//schreibe den Temperaturwert auf den Mqtt-Broker im Topic terrarium/temp1

  
}

//06.01.2020 Zum ersten mal eingesetz und seit laden falst alle Serial print deaktivert plus Durchkommentiert
