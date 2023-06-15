#include <DHT.h>
#include <SoftwareSerial.h>

const String PID = "#YOUR_ARDUINO_PID#";
const String VID = "#YOUR_ARDUINO_VID#";

String wifiName = "#YOUR_WIFI_NAME#";          // We write the name of our network here.
String wifiPassword = "#YOUR_WIFI_PASSWORD#";  // We write the password of our network here.

int rxPin = 3;  //ESP8266 RX pini
int txPin = 2;  //ESP8266 TX pini

String ip = "#YOUR_URL_OR_LINK#";  // link or ip address
String url = "#YOUR_API_URL#";

SoftwareSerial esp(rxPin, txPin);  // We make serial communication pin settings.

// Definimos el pin digital donde se conecta el sensor
#define DHTPIN 4
// Dependiendo del tipo de sensor
#define DHTTYPE DHT11

// Inicializamos el sensor DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {

  Serial.begin(9600);  // We are starting our communication with the serial port.
  Serial.println("Started");

  // Comenzamos el sensor DHT
  dht.begin();

  esp.begin(115200);  // We are starting serial communication with ESP8266.
  esp.println("AT");  // We do the module control with the AT command.
  Serial.println("AT sent");

  while (!esp.find("OK")) {  // We wait until the module is ready.
    esp.println("AT");
    Serial.println("ESP8266 Not Find.");
  }

  Serial.println("OK Command Received");
  esp.println("AT+CWMODE=1");  // We set the ESP8266 module as a client.

  while (!esp.find("OK")) {    // We wait until the setting is done.
    esp.println("AT+CWMODE=1");
    Serial.println("Setting is ....");
  }

  Serial.println("Set as client");
  Serial.println("Connecting to the Network ...");
  esp.println("AT+CWJAP=\"" + wifiName + "\",\"" + wifiPassword + "\"");  // We are connecting to our network.

  while (!esp.find("OK"))
    ;  // We wait until it is connected to the network.
  Serial.println("connected to the network.");
  delay(1000);

}
void loop() {

  esp.println("AT+CIPSTART=\"TCP\",\"" + ip + "\",80");  // We connect to Thingspeak.
  if (esp.find("Error")) {                               // We check the connection error.
    Serial.println("AT+CIPSTART Error");
  }

  // Leemos la humedad relativa
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  String temperatura = String(t, 2);
  String humedad = String(h, 2);

  String data = "GET " + url + "?PID=" + PID + "&VID=" + VID;
  data += "&t=";
  data += temperatura;  // The temperature variable we will send
  data += "&h=";
  data += humedad;  // The moisture variable we will send
  data += " HTTP/1.1";
  data += "\r\n\r\n";
  
  esp.print("AT+CIPSEND=");  // We give the length of data that we will send to ESP.
  esp.println(data.length() + 2);
  delay(2000);
  
  if (esp.find(">")) {  // The commands in it are running when ESP8266 is ready..
    esp.print(data);    // We send the data.
    Serial.println(data);
    Serial.println("Data sent.");
    delay(1000);
  }

  Serial.println("Connection Closed.");
  esp.println("AT+CIPCLOSE");
  delay(1000);

}