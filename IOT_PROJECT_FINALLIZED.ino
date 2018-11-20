// inspiration taken from the whole internet, thanks stackoverflow
// Libraries
#define BLYNK_PRINT Serial//this is so important that i had to place it here .D
#include <ESP8266WiFi.h> // main library for the microcontroller we use
#include <BlynkSimpleEsp8266.h> //main library for the application we use
#include <DHT.h> // main library(community ver) for the dht sensor
#include "DHT.h" //uhm lets pretend this does anything
#include <Adafruit_Sensor.h> // needed for compatability with all adafruit products :D its just like modding 
#include <Servo.h> //the main servo library, very cool, thank u.


// authendication for blynk, ssid and password for common wifi.
char auth[] = "4aca17fa9943410c86d6e13921790bfe"; // changes with every new project, keep updated
const char* ssid = "GetOffMyLan"; //school network = GetOffMyLan, Homenetwork = HJEM_2G
const char* password = "FuckOffPlz"; //school network = FuckOffPlz, Homenetwork = Ludvig48

// who is hosting the client for sending
const char* host = "dweet.io";

// Analog pins(lol theres none)

// The DHT sensor pins and defines
#define DHTTYPE DHT11   // DHT 11 model, which we are using
#define DHTPIN 5      // the pin used for it
DHT dht(DHTPIN, DHTTYPE, 15); //no idea why the 3rd parameter is there its not even used :D
//the two other parameters explain them self 
 
 // all servo stuff
const int servoPin = 2; //this is only here becaus its hard to look for in setup
Servo servo; // including the servo commands and functions

//all blynk functions
BlynkTimer timer; // Announcing the timer "AAAAND IN THE LEFT CORNORRRRR, TIMEERRRRR"
BLYNK_WRITE(V0) {// using virtual 0
  servo.write(param.asInt()); //blynk servo control
}

BLYNK_WRITE(V3){  // using virtual 3
  if(param.asInt()){  // as long as the value is 1, go high
    digitalWrite(13, HIGH);
  }
  else{ // however if value is 0 go low
    digitalWrite(13, LOW);
  }
}

void myTimerEvent(){
  float t = dht.readTemperature();
  Blynk.virtualWrite(V1, t);
  float h = dht.readHumidity();
  Blynk.virtualWrite(V2, h);
}

void setup() {
  pinMode(13, OUTPUT); // this is the pin for the fan 
  // Start Serial
  Serial.begin(115200);
  delay(10);
  dht.begin();
  servo.attach (servoPin);
  Blynk.begin(auth, ssid, password);
  // We start by connecting to a WiFi network
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println(WiFi.localIP()); //the ip assigned to the device and used for localhost
  timer.setInterval(1000L, myTimerEvent); //and a timer for time

}

void loop() {
  Blynk.run(); //for running the connection to blynk, very ez im looking at you dweet.
  timer.run(); // a timer for time :D
  WiFiClient client; // my client setup
  const int httpPort = 80; //local host
  if (!client.connect(host, httpPort)) { //now i can connect to it locally
    return;
  }
//read everything here
  int a = analogRead(A0); //Photoresistor
  float h = dht.readHumidity(); //well look at the name
  float t = dht.readTemperature();  //again, very nice name

// This will send the request to the server, copy pasta from dweet github :)
  client.print(String("GET /dweet/for/myesp8266?ligtintesity=") + String(a) + "&humidity=" + String(h) + "&temperature=" + String(t) + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" +
               "Connection: close\r\n\r\n");

  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  // so freeware doesnt like me spamming so heres a mandatory delay of 10 seconds, im sry server.
  delay(10000);
}
